#include "CollisionManager.h"

#include <algorithm>
#include <cassert>

#ifdef _DEBUG

#include <iostream>
#include <format> 

#endif

void CollisionManager::Clear() {

	bodies_.clear();

}

void CollisionManager::AddBody(HitMesh* body) {

	assert(body != nullptr);

	if (body == nullptr) {
		return;
	}

	for (HitMesh* registered : bodies_) {

		if (registered == body) {

			return;

		}

	}

	bodies_.push_back(body);

}

void CollisionManager::CheckCollision() {

	for (HitMesh* body : bodies_) {

		if (body != nullptr) {
			body->UpdateMatrix();
		}

	}

	for (size_t i = 0; i < bodies_.size(); ++i) {

		HitMesh* meshA = bodies_[i];

		if (meshA == nullptr) {
			continue;
		}

		for (size_t j = i + 1; j < bodies_.size(); ++j) {

			HitMesh* meshB = bodies_[j];

			if (meshB == nullptr) {
				continue;
			}

			SATResult result = TestSAT(*meshA, *meshB);

			if (!result.isHit) {
				continue;
			}

			ResolveCollision(*meshA, *meshB, result);

			meshA->UpdateMatrix();
			meshB->UpdateMatrix();

		}

	}

}

void CollisionManager::ResolveCollision(
	HitMesh& meshA,
	HitMesh& meshB,
	const SATResult& result) {

	ResolvePosition(meshA, meshB, result);

	if (ShouldApplyImpactImpulse(meshA, meshB, result)) {
		ResolveVelocity(meshA, meshB, result);
	}
}

void CollisionManager::ResolvePosition(
	HitMesh& meshA,
	HitMesh& meshB,
	const SATResult& result) {

	float totalInverseMass =
		meshA.inverseMass + meshB.inverseMass;

	if (totalInverseMass <= 0.0f) {
		return;
	}

	constexpr float kSlop = 0.001f;
	constexpr float kPercent = 1.0f;

	float correctionDepth =
		std::max(result.depth - kSlop, 0.0f);

	Vector3 correction =
		result.normal *
		(correctionDepth / totalInverseMass) *
		kPercent;

	meshA.position += correction * meshA.inverseMass;
	meshB.position -= correction * meshB.inverseMass;

}

void CollisionManager::ResolveVelocity(
	HitMesh& meshA,
	HitMesh& meshB,
	const SATResult& result) {

	float totalInverseMass =
		meshA.inverseMass + meshB.inverseMass;

	if (totalInverseMass <= 0.0f) {
		return;
	}

	Vector3 normal =
		VectorNormalize(result.normal);

	Vector3 contactPoint =
		result.contactPoint;

	Vector3 rA =
		contactPoint - meshA.GetCenter();

	Vector3 rB =
		contactPoint - meshB.GetCenter();

	Vector3 vA =
		meshA.velocity +
		VectorCross(meshA.angularVelocity, rA);

	Vector3 vB =
		meshB.velocity +
		VectorCross(meshB.angularVelocity, rB);

	Vector3 relativeVelocity = vB - vA;

	float velocityAlongNormal =
		VectorDot(relativeVelocity, normal);


	float restitution =
		std::min(meshA.restitution, meshB.restitution);

	constexpr float kRestThreshold = 0.5f;

	if (std::abs(velocityAlongNormal) < kRestThreshold) {
		restitution = 0.0f;
	}

	Vector3 rACrossN =
		VectorCross(rA, normal);

	Vector3 rBCrossN =
		VectorCross(rB, normal);

	Vector3 inertiaA =
		meshA.inverseInertiaTensorWorld * rACrossN;

	Vector3 inertiaB =
		meshB.inverseInertiaTensorWorld * rBCrossN;

	Vector3 angularA =
		VectorCross(inertiaA, rA);

	Vector3 angularB =
		VectorCross(inertiaB, rB);

	float angularFactor =
		VectorDot(normal, angularA + angularB);

	float denominator =
		totalInverseMass + angularFactor;

	if (denominator <= 0.000001f) {
		return;
	}

	float impulseScalar = 0.0f;

	Vector3 impulse = {};

	if (velocityAlongNormal >= 0.0f)
	{

		impulseScalar = -(1.0f + restitution) * velocityAlongNormal;

		impulseScalar /= denominator;

		impulse = normal* impulseScalar;

		if (meshA.inverseMass > 0.0f) {
			meshA.velocity -= impulse * meshA.inverseMass;
			meshA.angularVelocity -=
				meshA.inverseInertiaTensorWorld *
				VectorCross(rA, impulse);
		}

		if (meshB.inverseMass > 0.0f) {
			meshB.velocity += impulse * meshB.inverseMass;
			meshB.angularVelocity +=
				meshB.inverseInertiaTensorWorld *
				VectorCross(rB, impulse);
		}

	}

	// ------------------------------
// Friction Impulse
// ------------------------------

	vA = meshA.velocity + VectorCross(meshA.angularVelocity, rA);
	vB = meshB.velocity + VectorCross(meshB.angularVelocity, rB);

	relativeVelocity = vB - vA;

	Vector3 tangent =
		relativeVelocity -
		normal * VectorDot(relativeVelocity, normal);

	if (VectorLength(tangent) > 0.000001f) {

		tangent = VectorNormalize(tangent);

		Vector3 rACrossT = VectorCross(rA, tangent);
		Vector3 rBCrossT = VectorCross(rB, tangent);

		Vector3 inertiaTA = meshA.inverseInertiaTensorWorld * rACrossT;
		Vector3 inertiaTB = meshB.inverseInertiaTensorWorld * rBCrossT;

		float frictionDenominator =
			totalInverseMass +
			VectorDot(
				tangent,
				VectorCross(inertiaTA, rA) +
				VectorCross(inertiaTB, rB));

		if (frictionDenominator > 0.000001f) {

			float tangentSpeed =
				VectorDot(relativeVelocity, tangent);

			float jt =
				-tangentSpeed / frictionDenominator;

			float staticFriction =
				std::sqrt(meshA.friction * meshB.friction);

			float dynamicFriction =
				staticFriction * 0.6f;

			Vector3 frictionImpulse{};

			float normalImpulseForFriction =
				std::max(std::abs(impulseScalar), result.depth * 0.05f);

			if (std::abs(jt) <= normalImpulseForFriction * staticFriction) {

				frictionImpulse = tangent * jt;

			} else {

				frictionImpulse =
					tangent *
					(-std::copysign(
						normalImpulseForFriction * dynamicFriction,
						tangentSpeed));
			}

			if (meshA.inverseMass > 0.0f) {
				meshA.velocity -= frictionImpulse * meshA.inverseMass;
				meshA.angularVelocity -=
					meshA.inverseInertiaTensorWorld *
					VectorCross(rA, frictionImpulse);
			}

			if (meshB.inverseMass > 0.0f) {
				meshB.velocity += frictionImpulse * meshB.inverseMass;
				meshB.angularVelocity +=
					meshB.inverseInertiaTensorWorld *
					VectorCross(rB, frictionImpulse);
			}
		}
	}

#ifdef _DEBUG

	Vector3 torqueB = VectorCross(rB, impulse);

	std::cout << std::format(
		"rB=({:.3f},{:.3f},{:.3f}) torqueB=({:.3f},{:.3f},{:.3f})",
		rB.x, rB.y, rB.z,
		torqueB.x, torqueB.y, torqueB.z)
		<< std::endl;

	std::cout << std::format(
		"vn = {:.6f}, impulse = {:.6f}",
		velocityAlongNormal,
		impulseScalar)
		<< std::endl;

	std::cout << std::format(
		"I^-1 = [{:.6f}, {:.6f}, {:.6f}]",
		meshA.inverseInertiaTensorWorld.m[0][0],
		meshA.inverseInertiaTensorWorld.m[1][1],
		meshA.inverseInertiaTensorWorld.m[2][2])
		<< std::endl;

	std::cout << std::format(
		"A invMass={}, I^-1=[{:.6f}, {:.6f}, {:.6f}]",
		meshA.inverseMass,
		meshA.inverseInertiaTensorWorld.m[0][0],
		meshA.inverseInertiaTensorWorld.m[1][1],
		meshA.inverseInertiaTensorWorld.m[2][2])
		<< std::endl;

	std::cout << std::format(
		"B invMass={}, I^-1=[{:.6f}, {:.6f}, {:.6f}]",
		meshB.inverseMass,
		meshB.inverseInertiaTensorWorld.m[0][0],
		meshB.inverseInertiaTensorWorld.m[1][1],
		meshB.inverseInertiaTensorWorld.m[2][2])
		<< std::endl;

#endif

}

bool CollisionManager::ShouldApplyImpactImpulse(
	HitMesh& meshA,
	HitMesh& meshB,
	const SATResult& result) {

	Vector3 normal =
		VectorNormalize(result.normal);

	Vector3 contactPoint =
		result.contactPoint;

	Vector3 rA =
		contactPoint - meshA.GetCenter();

	Vector3 rB =
		contactPoint - meshB.GetCenter();

	Vector3 vA =
		meshA.velocity +
		VectorCross(meshA.angularVelocity, rA);

	Vector3 vB =
		meshB.velocity +
		VectorCross(meshB.angularVelocity, rB);

	Vector3 relativeVelocity =
		vB - vA;

	float vn =
		VectorDot(relativeVelocity, normal);

	constexpr float kImpactVelocity = 0.35f;

	return vn > kImpactVelocity;
}