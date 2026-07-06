#include "ContactConstraint.h"

#include <algorithm>
#include <cmath>

void ContactConstraint::Initialize(
    HitMesh* a,
    HitMesh* b,
    const SATResult& sat) {

    bodyA = a;
    bodyB = b;

    normal = VectorNormalize(sat.normal);

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

    ContactPoint& contact =
        contacts[contactIndex];

    Vector3 rA =
        contact.position - bodyA->GetCenter();

    Vector3 rB =
        contact.position - bodyB->GetCenter();

    Vector3 rACrossN =
        VectorCross(rA, normal);

    Vector3 rBCrossN =
        VectorCross(rB, normal);

    float denominator =
        totalInverseMass +
        VectorDot(
            normal,
            VectorCross(bodyA->inverseInertiaTensorWorld * rACrossN, rA) +
            VectorCross(bodyB->inverseInertiaTensorWorld * rBCrossN, rB));

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

    Vector3 correctionImpulse =
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

    Vector3 rA =
        contact.position - bodyA->GetCenter();

    Vector3 rB =
        contact.position - bodyB->GetCenter();

    Vector3 vA =
        bodyA->velocity +
        VectorCross(bodyA->angularVelocity, rA);

    Vector3 vB =
        bodyB->velocity +
        VectorCross(bodyB->angularVelocity, rB);

    Vector3 relativeVelocity =
        vB - vA;

    float vn =
        VectorDot(relativeVelocity, normal);

    if (vn > 0.0f) {
        return;
    }

    Vector3 rACrossN =
        VectorCross(rA, normal);

    Vector3 rBCrossN =
        VectorCross(rB, normal);

    float normalDenominator =
        totalInverseMass +
        VectorDot(
            normal,
            VectorCross(bodyA->inverseInertiaTensorWorld * rACrossN, rA) +
            VectorCross(bodyB->inverseInertiaTensorWorld * rBCrossN, rB));

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

    rA =
        contact.position - bodyA->GetCenter();

    rB =
        contact.position - bodyB->GetCenter();

    vA =
        bodyA->velocity +
        VectorCross(bodyA->angularVelocity, rA);

    vB =
        bodyB->velocity +
        VectorCross(bodyB->angularVelocity, rB);

    relativeVelocity =
        vB - vA;

    Vector3 tangent =
        relativeVelocity -
        normal * VectorDot(relativeVelocity, normal);

    if (VectorLength(tangent) <= 0.000001f) {
        return;
    }

    tangent =
        VectorNormalize(tangent);

    Vector3 rACrossT =
        VectorCross(rA, tangent);

    Vector3 rBCrossT =
        VectorCross(rB, tangent);

    float tangentDenominator =
        totalInverseMass +
        VectorDot(
            tangent,
            VectorCross(bodyA->inverseInertiaTensorWorld * rACrossT, rA) +
            VectorCross(bodyB->inverseInertiaTensorWorld * rBCrossT, rB));

    if (tangentDenominator <= 0.000001f) {
        return;
    }

    float tangentImpulse =
        -VectorDot(relativeVelocity, tangent) / tangentDenominator;

    float maxFriction =
        friction * contact.accumulatedNormalImpulse;

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
    const Vector3& correctionImpulse,
    const Vector3& point) {

    Vector3 rA =
        point - bodyA->GetCenter();

    Vector3 rB =
        point - bodyB->GetCenter();

    if (bodyA->inverseMass > 0.0f) {

        bodyA->position -=
            correctionImpulse * bodyA->inverseMass;

        Vector3 angularCorrection =
            bodyA->inverseInertiaTensorWorld *
            VectorCross(rA, correctionImpulse);

        angularCorrection =
            -angularCorrection;

        float angle =
            VectorLength(angularCorrection);

        if (angle > 0.000001f) {

            Vector3 axis =
                angularCorrection / angle;

            Quaternion dq =
                Quaternion::FromAxisAngle(axis, angle);

            bodyA->rotation =
                (dq * bodyA->rotation).normalized();
        }
    }

    if (bodyB->inverseMass > 0.0f) {

        bodyB->position +=
            correctionImpulse * bodyB->inverseMass;

        Vector3 angularCorrection =
            bodyB->inverseInertiaTensorWorld *
            VectorCross(rB, correctionImpulse);

        float angle =
            VectorLength(angularCorrection);

        if (angle > 0.000001f) {

            Vector3 axis =
                angularCorrection / angle;

            Quaternion dq =
                Quaternion::FromAxisAngle(axis, angle);

            bodyB->rotation =
                (dq * bodyB->rotation).normalized();
        }
    }
}

void ContactConstraint::ApplyImpulse(
    const Vector3& impulse,
    const Vector3& point) {

    Vector3 rA =
        point - bodyA->GetCenter();

    Vector3 rB =
        point - bodyB->GetCenter();

    if (bodyA->inverseMass > 0.0f) {

        bodyA->velocity -=
            impulse * bodyA->inverseMass;

        bodyA->angularVelocity -=
            bodyA->inverseInertiaTensorWorld *
            VectorCross(rA, impulse);
    }

    if (bodyB->inverseMass > 0.0f) {

        bodyB->velocity +=
            impulse * bodyB->inverseMass;

        bodyB->angularVelocity +=
            bodyB->inverseInertiaTensorWorld *
            VectorCross(rB, impulse);
    }
}