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

    ResolveAngularVelocity(meshA, meshB, result);

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

    Vector3 relativeVelocity =
        meshA.velocity - meshB.velocity;

    float velocityAlongNormal =
        VectorDot(relativeVelocity, result.normal);

    if (velocityAlongNormal > 0.0f) {
        return;
    }

    constexpr float kRestThreshold = 0.5f;

    float restitution =
        std::min(meshA.restitution, meshB.restitution);

    if (std::abs(velocityAlongNormal) < kRestThreshold) {
        restitution = 0.0f;
    }

    float impulseScalar =
        -(1.0f + restitution) * velocityAlongNormal;

    impulseScalar /= totalInverseMass;

    Vector3 impulse =
        result.normal * impulseScalar;

    meshA.velocity += impulse * meshA.inverseMass;
    meshB.velocity -= impulse * meshB.inverseMass;
}

void CollisionManager::ResolveAngularVelocity(
    HitMesh& meshA,
    HitMesh& meshB,
    const SATResult& result) {

    constexpr float kMaxAngularSpeed = 3.0f;

    Vector3 relativeVelocity =
        meshA.velocity - meshB.velocity;

    float normalVelocity =
        VectorDot(relativeVelocity, result.normal);

    if (normalVelocity > 0.0f) {
        return;
    }

    Vector3 impulse =
        result.normal * (-normalVelocity);

    Vector3 contactPoint =
        result.contactPoint;

    if (meshA.inverseMass > 0.0f) {

        Vector3 rA =
            contactPoint - meshA.GetCenter();

        Vector3 torqueA =
            VectorCross(rA, impulse);

        Vector3 deltaAngularVelocityA =
            meshA.inverseInertiaTensorWorld * torqueA;

        meshA.angularVelocity += deltaAngularVelocityA;

        float speed = VectorLength(meshA.angularVelocity);

        if (speed > kMaxAngularSpeed) {
            meshA.angularVelocity =
                VectorNormalize(meshA.angularVelocity) * kMaxAngularSpeed;
        }
    }

    if (meshB.inverseMass > 0.0f) {

        Vector3 rB =
            contactPoint - meshB.GetCenter();

        Vector3 torqueB =
            VectorCross(rB, impulse * -1.0f);

        Vector3 deltaAngularVelocityB =
            meshB.inverseInertiaTensorWorld * torqueB;

        meshB.angularVelocity += deltaAngularVelocityB;

        float speed = VectorLength(meshB.angularVelocity);

        if (speed > kMaxAngularSpeed) {
            meshB.angularVelocity =
                VectorNormalize(meshB.angularVelocity) * kMaxAngularSpeed;
        }
    }
}