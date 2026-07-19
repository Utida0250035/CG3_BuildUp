#pragma once

#include <vector>

#include "./CollisionTypes.h"
#include "./HitMesh.h"

namespace Atrum::Physics {

    struct ContactConstraint {

        HitMesh* bodyA = nullptr;
        HitMesh* bodyB = nullptr;

        Math::Vector3 normal{};

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
            const Math::Vector3& correctionImpulse,
            const Math::Vector3& point);

        void ApplyImpulse(
            const Math::Vector3& impulse,
            const Math::Vector3& point);
    };

}