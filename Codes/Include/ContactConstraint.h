#pragma once

#include <vector>

#include "HitMesh.h"
#include "CollisionTypes.h"

struct ContactConstraint {

    HitMesh* bodyA = nullptr;
    HitMesh* bodyB = nullptr;

    Vector3 normal{};

    std::vector<ContactPoint> contacts;

    float restitution = 0.0f;
    float friction = 0.0f;

    void Initialize(
        HitMesh* a,
        HitMesh* b,
        const SATResult& sat);

    void SolvePosition();

    void SolveVelocity();

private:

    void SolvePositionAtPoint(
        size_t contactIndex);

    void SolveVelocityAtPoint(
        size_t contactIndex);

    void ApplyPositionCorrection(
        const Vector3& correctionImpulse,
        const Vector3& point);

    void ApplyImpulse(
        const Vector3& impulse,
        const Vector3& point);
};