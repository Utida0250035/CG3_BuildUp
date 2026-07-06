#include "CollisionManager.h"

#include "SAT.h"

void CollisionManager::Clear() {

    bodies_.clear();
    constraints_.clear();
}

void CollisionManager::AddBody(HitMesh* body) {

    if (body == nullptr) {
        return;
    }

    bodies_.push_back(body);
}

void CollisionManager::CheckCollision() {

    constraints_.clear();

    for (HitMesh* body : bodies_) {

        if (body == nullptr) {
            continue;
        }

        body->UpdateMatrix();
    }

    BuildConstraints();

    SolvePositions();

    SolveVelocities();
}

void CollisionManager::BuildConstraints() {

    const size_t bodyCount = bodies_.size();

    for (size_t i = 0; i < bodyCount; ++i) {

        HitMesh* bodyA = bodies_[i];

        if (bodyA == nullptr) {
            continue;
        }

        for (size_t j = i + 1; j < bodyCount; ++j) {

            HitMesh* bodyB = bodies_[j];

            if (bodyB == nullptr) {
                continue;
            }

            if (bodyA->inverseMass <= 0.0f &&
                bodyB->inverseMass <= 0.0f) {
                continue;
            }

            SATResult sat = TestSAT(*bodyA, *bodyB);

            if (!sat.hit) {
                continue;
            }

            Vector3 centerA = bodyA->GetCenter();
            Vector3 centerB = bodyB->GetCenter();

            Vector3 centerDirection = centerB - centerA;

            if (VectorDot(centerDirection, sat.normal) < 0.0f) {
                sat.normal = -sat.normal;
            }

            ContactConstraint constraint{};
            constraint.Initialize(bodyA, bodyB, sat);

            constraints_.push_back(constraint);
        }
    }
}

void CollisionManager::SolvePositions() {

    constexpr int kPositionIterations = 4;

    for (int i = 0; i < kPositionIterations; ++i) {

        for (ContactConstraint& constraint : constraints_) {
            constraint.SolvePosition();
        }
    }
}

void CollisionManager::SolveVelocities() {

    constexpr int kVelocityIterations = 8;

    for (int i = 0; i < kVelocityIterations; ++i) {

        for (ContactConstraint& constraint : constraints_) {
            constraint.SolveVelocity();
        }
    }
}