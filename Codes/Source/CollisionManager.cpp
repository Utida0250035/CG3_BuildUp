#include "CollisionManager.h"

#include <algorithm>
#include <cassert>

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

	ResolveVelocity(meshA, meshB, result);

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

	Vector3 relativeVelocity =
		vA - vB;

	float velocityAlongNormal =
		VectorDot(relativeVelocity, normal);

	if (velocityAlongNormal > 0.0f) {
		return;
	}

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

	float impulseScalar =
		-(1.0f + restitution) * velocityAlongNormal;

	impulseScalar /= denominator;

	Vector3 impulse =
		normal * impulseScalar;

	if (meshA.inverseMass > 0.0f) {

		meshA.velocity +=
			impulse * meshA.inverseMass;

		meshA.angularVelocity +=
			meshA.inverseInertiaTensorWorld *
			VectorCross(rA, impulse);
	}

	if (meshB.inverseMass > 0.0f) {

		meshB.velocity -=
			impulse * meshB.inverseMass;

		meshB.angularVelocity -=
			meshB.inverseInertiaTensorWorld *
			VectorCross(rB, impulse);
	}
}