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

    contactPoint = sat.contactPoint;
    contactPoints = sat.contactPoints;

    if (contactPoints.empty()) {
        contactPoints.push_back(contactPoint);
    }

    penetration = sat.depth;

    restitution =
        std::min(bodyA->restitution, bodyB->restitution);

    friction =
        std::sqrt(bodyA->friction * bodyB->friction);

    accumulatedNormalImpulse = 0.0f;
    accumulatedTangentImpulse = 0.0f;

    accumulatedNormalImpulses.clear();
    accumulatedTangentImpulses.clear();

    accumulatedNormalImpulses.resize(contactPoints.size(), 0.0f);
    accumulatedTangentImpulses.resize(contactPoints.size(), 0.0f);
}

void ContactConstraint::SolvePosition() {

    if (bodyA == nullptr || bodyB == nullptr) {
        return;
    }

    float totalInverseMass =
        bodyA->inverseMass + bodyB->inverseMass;

    if (totalInverseMass <= 0.0f) {
        return;
    }

    constexpr float kSlop = 0.001f;
    constexpr float kPercent = 0.8f;

    float correctionDepth =
        std::max(penetration - kSlop, 0.0f);

    Vector3 correction =
        normal *
        (correctionDepth / totalInverseMass) *
        kPercent;

    bodyA->position -= correction * bodyA->inverseMass;
    bodyB->position += correction * bodyB->inverseMass;

    bodyA->UpdateMatrix();
    bodyB->UpdateMatrix();
}

void ContactConstraint::SolveVelocity() {

    if (bodyA == nullptr || bodyB == nullptr) {
        return;
    }

    if (contactPoints.empty()) {
        return;
    }

    for (size_t i = 0; i < contactPoints.size(); ++i) {
        SolveVelocityAtPoint(i);
    }

    accumulatedNormalImpulse = 0.0f;
    accumulatedTangentImpulse = 0.0f;

    for (float impulse : accumulatedNormalImpulses) {
        accumulatedNormalImpulse += impulse;
    }

    for (float impulse : accumulatedTangentImpulses) {
        accumulatedTangentImpulse += impulse;
    }
}

void ContactConstraint::SolveVelocityAtPoint(
    size_t contactIndex) {

    if (contactIndex >= contactPoints.size()) {
        return;
    }

    float totalInverseMass =
        bodyA->inverseMass + bodyB->inverseMass;

    if (totalInverseMass <= 0.0f) {
        return;
    }

    const Vector3& point = contactPoints[contactIndex];

    Vector3 rA =
        point - bodyA->GetCenter();

    Vector3 rB =
        point - bodyB->GetCenter();

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

    constexpr float kRestThreshold = 0.5f;

    if (std::abs(vn) < kRestThreshold) {
        e = 0.0f;
    }

    float contactCount =
        static_cast<float>(contactPoints.size());

    float normalImpulse =
        -(1.0f + e) * vn / normalDenominator;

    normalImpulse /= contactCount;

    float oldNormalImpulse =
        accumulatedNormalImpulses[contactIndex];

    accumulatedNormalImpulses[contactIndex] =
        std::max(oldNormalImpulse + normalImpulse, 0.0f);

    normalImpulse =
        accumulatedNormalImpulses[contactIndex] - oldNormalImpulse;

    ApplyImpulse(normal * normalImpulse, point);

    rA = point - bodyA->GetCenter();
    rB = point - bodyB->GetCenter();

    vA =
        bodyA->velocity +
        VectorCross(bodyA->angularVelocity, rA);

    vB =
        bodyB->velocity +
        VectorCross(bodyB->angularVelocity, rB);

    relativeVelocity = vB - vA;

    Vector3 tangent =
        relativeVelocity -
        normal * VectorDot(relativeVelocity, normal);

    if (VectorLength(tangent) <= 0.000001f) {
        return;
    }

    tangent = VectorNormalize(tangent);

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

    tangentImpulse /= contactCount;

    float maxFriction =
        friction * accumulatedNormalImpulses[contactIndex];

    float oldTangentImpulse =
        accumulatedTangentImpulses[contactIndex];

    accumulatedTangentImpulses[contactIndex] =
        std::clamp(
            oldTangentImpulse + tangentImpulse,
            -maxFriction,
            maxFriction);

    tangentImpulse =
        accumulatedTangentImpulses[contactIndex] - oldTangentImpulse;

    ApplyImpulse(tangent * tangentImpulse, point);
}

void ContactConstraint::ApplyImpulse(
    const Vector3& impulse,
    const Vector3& point) {

    Vector3 rA =
        point - bodyA->GetCenter();

    Vector3 rB =
        point - bodyB->GetCenter();

    if (bodyA->inverseMass > 0.0f) {

        bodyA->velocity -= impulse * bodyA->inverseMass;

        bodyA->angularVelocity -=
            bodyA->inverseInertiaTensorWorld *
            VectorCross(rA, impulse);
    }

    if (bodyB->inverseMass > 0.0f) {

        bodyB->velocity += impulse * bodyB->inverseMass;

        bodyB->angularVelocity +=
            bodyB->inverseInertiaTensorWorld *
            VectorCross(rB, impulse);
    }
}