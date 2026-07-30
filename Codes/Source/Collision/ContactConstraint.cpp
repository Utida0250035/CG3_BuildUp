#include "Engine/Alias/PhysicsAlias.h"
#include "Collision/ContactConstraint.h"
#include <algorithm>
#include <cmath>

namespace Atrum::Physics {

	void ContactConstraint::Initialize(
		HitMesh* a,
		HitMesh* b,
		const SATResult& sat) {

		bodyA = a;
		bodyB = b;

		normal = sat.normal.Normalized();

		contacts = sat.contactPoints;

		restitution =
			std::min(bodyA->restitution, bodyB->restitution);

		friction =
			std::sqrt(bodyA->friction * bodyB->friction);
	}

	void ContactConstraint::SolvePosition() {

		if (bodyA == nullptr || bodyB == nullptr) {
			return;
		}

		if (contacts.empty()) {
			return;
		}

		for (size_t i = 0; i < contacts.size(); ++i) {
			SolvePositionAtPoint(i);
		}

		bodyA->UpdateMatrix();
		bodyB->UpdateMatrix();
	}

	void ContactConstraint::SolvePositionAtPoint(
		size_t contactIndex) {

		if (contactIndex >= contacts.size()) {
			return;
		}

		float totalInverseMass =
			bodyA->inverseMass + bodyB->inverseMass;

		if (totalInverseMass <= 0.0f) {
			return;
		}

		ContactPoint& contact = contacts[contactIndex];

		M::Vector3 rA = contact.position - bodyA->GetCenter();

		M::Vector3 rB = contact.position - bodyB->GetCenter();

		M::Vector3 rACrossN = Cross(rA, normal);

		M::Vector3 rBCrossN = Cross(rB, normal);

		float denominator =
			totalInverseMass +
			normal.Dot(
				Cross(bodyA->inverseInertiaTensorWorld * rACrossN, rA)
				+ Cross(bodyB->inverseInertiaTensorWorld * rBCrossN, rB)
			);

		if (denominator <= 0.000001f) {
			return;
		}

		constexpr float kSlop = 0.001f;
		constexpr float kPercent = 0.1f;

		float correctionDepth =
			std::max(contact.penetration - kSlop, 0.0f);

		if (correctionDepth <= 0.0f) {
			return;
		}

		float impulseMagnitude =
			correctionDepth * kPercent / denominator;

		M::Vector3 correctionImpulse =
			normal * impulseMagnitude;

		ApplyPositionCorrection(
			correctionImpulse,
			contact.position);
	}

	void ContactConstraint::SolveVelocity() {

		if (bodyA == nullptr || bodyB == nullptr) {
			return;
		}

		if (contacts.empty()) {
			return;
		}

		for (size_t i = 0; i < contacts.size(); ++i) {
			SolveVelocityAtPoint(i);
		}
	}

	void ContactConstraint::SolveVelocityAtPoint(
		size_t contactIndex) {

		if (contactIndex >= contacts.size()) {
			return;
		}

		float totalInverseMass =
			bodyA->inverseMass + bodyB->inverseMass;

		if (totalInverseMass <= 0.0f) {
			return;
		}

		ContactPoint& contact =
			contacts[contactIndex];

		M::Vector3 rA =
			contact.position - bodyA->GetCenter();

		M::Vector3 rB =
			contact.position - bodyB->GetCenter();

		M::Vector3 vA = bodyA->velocity + Cross(bodyA->angularVelocity, rA);

		M::Vector3 vB = bodyB->velocity + Cross(bodyB->angularVelocity, rB);

		M::Vector3 relativeVelocity = vB - vA;

		float vn = relativeVelocity.Dot(normal);

		if (vn > 0.0f) {
			return;
		}

		M::Vector3 rACrossN = Cross(rA, normal);

		M::Vector3 rBCrossN = Cross(rB, normal);

		float normalDenominator = totalInverseMass + normal.Dot(
			Cross(bodyA->inverseInertiaTensorWorld * rACrossN, rA)
			+ Cross(bodyB->inverseInertiaTensorWorld * rBCrossN, rB)
		);

		if (normalDenominator <= 0.000001f) {
			return;
		}

		float e = restitution;

		constexpr float kRestThreshold = 1.0f;

		if (std::abs(vn) < kRestThreshold) {
			e = 0.0f;
		}

		float normalImpulse =
			-(1.0f + e) * vn / normalDenominator;

		float oldNormalImpulse =
			contact.accumulatedNormalImpulse;

		contact.accumulatedNormalImpulse =
			std::max(
				oldNormalImpulse + normalImpulse,
				0.0f);

		normalImpulse =
			contact.accumulatedNormalImpulse - oldNormalImpulse;

		ApplyImpulse(
			normal * normalImpulse,
			contact.position);

		rA = contact.position - bodyA->GetCenter();

		rB = contact.position - bodyB->GetCenter();

		vA = bodyA->velocity + Cross(bodyA->angularVelocity, rA);

		vB = bodyB->velocity + Cross(bodyB->angularVelocity, rB);

		relativeVelocity = vB - vA;

		M::Vector3 tangent = relativeVelocity - normal * relativeVelocity.Dot(normal);

		if (tangent.Length() <= 0.000001f) {
			return;
		}

		tangent.Normalize();

		M::Vector3 rACrossT = Cross(rA, tangent);

		M::Vector3 rBCrossT = Cross(rB, tangent);

		float tangentDenominator = totalInverseMass + tangent.Dot(
			Cross(bodyA->inverseInertiaTensorWorld * rACrossT, rA)
			+ Cross(bodyB->inverseInertiaTensorWorld * rBCrossT, rB)
		);

		if (tangentDenominator <= 0.000001f) {
			return;
		}

		float tangentImpulse = -relativeVelocity.Dot(tangent) / tangentDenominator;

		float maxFriction = friction * contact.accumulatedNormalImpulse;

		float oldTangentImpulse =
			contact.accumulatedTangentImpulse;

		contact.accumulatedTangentImpulse =
			std::clamp(
				oldTangentImpulse + tangentImpulse,
				-maxFriction,
				maxFriction);

		tangentImpulse =
			contact.accumulatedTangentImpulse - oldTangentImpulse;

		ApplyImpulse(
			tangent * tangentImpulse,
			contact.position);
	}

	void ContactConstraint::ApplyPositionCorrection(
		const M::Vector3& correctionImpulse,
		const M::Vector3& point) {

		M::Vector3 rA =
			point - bodyA->GetCenter();

		M::Vector3 rB =
			point - bodyB->GetCenter();

		if (bodyA->inverseMass > 0.0f) {

			bodyA->position -=
				correctionImpulse * bodyA->inverseMass;

			M::Vector3 angularCorrection = -(bodyA->inverseInertiaTensorWorld * Cross(rA, correctionImpulse));

			float angle = angularCorrection.Length();

			if (angle > 0.000001f) {

				M::Vector3 axis = angularCorrection / angle;

				M::Quaternion dq = M::Quaternion::FromAxisAngle(axis, angle);

				bodyA->quaternion = (bodyA->quaternion * dq).Normalized();
			}
		}

		if (bodyB->inverseMass > 0.0f) {

			bodyB->position += correctionImpulse * bodyB->inverseMass;

			M::Vector3 angularCorrection = bodyB->inverseInertiaTensorWorld * Cross(rB, correctionImpulse);

			float angle = angularCorrection.Length();

			if (angle > 0.000001f) {

				M::Vector3 axis =
					angularCorrection / angle;

				M::Quaternion dq = M::Quaternion::FromAxisAngle(axis, angle);

				bodyB->quaternion = (bodyB->quaternion * dq).Normalized();

			}
		}
	}

	void ContactConstraint::ApplyImpulse(
		const M::Vector3& impulse,
		const M::Vector3& point) {

		M::Vector3 rA =
			point - bodyA->GetCenter();

		M::Vector3 rB =
			point - bodyB->GetCenter();

		if (bodyA->inverseMass > 0.0f) {

			bodyA->velocity -= impulse * bodyA->inverseMass;

			bodyA->angularVelocity -= bodyA->inverseInertiaTensorWorld * Cross(rA, impulse);

		}

		if (bodyB->inverseMass > 0.0f) {

			bodyB->velocity += impulse * bodyB->inverseMass;

			bodyB->angularVelocity += bodyB->inverseInertiaTensorWorld * Cross(rB, impulse);

		}
	}

}