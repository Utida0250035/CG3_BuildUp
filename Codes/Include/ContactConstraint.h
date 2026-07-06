#pragma once

#include <vector>

#include "HitMesh.h"
#include "CollisionTypes.h"

struct ContactConstraint {

    HitMesh* bodyA = nullptr;
    HitMesh* bodyB = nullptr;

    Vector3 normal{};

    Vector3 contactPoint{};
    std::vector<Vector3> contactPoints;

    float penetration = 0.0f;

    float restitution = 0.0f;
    float friction = 0.0f;

    float accumulatedNormalImpulse = 0.0f;
    float accumulatedTangentImpulse = 0.0f;

    std::vector<float> accumulatedNormalImpulses;
    std::vector<float> accumulatedTangentImpulses;

    void Initialize(
        HitMesh* a,
        HitMesh* b,
        const SATResult& sat);

    void SolvePosition();

    void SolveVelocity();

private:

    void SolveVelocityAtPoint(
        size_t contactIndex);

    void ApplyImpulse(
        const Vector3& impulse,
        const Vector3& point);
};