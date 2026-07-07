#pragma once

#include <vector>

#include "HitMesh.h"
#include "ContactConstraint.h"

class CollisionManager {
public:

    void Clear();

    void AddBody(HitMesh* body);

    void CheckCollision();

private:

    void BuildConstraints();

    void SolvePositions();

    void SolveVelocities();

private:

    std::vector<HitMesh*> bodies_;
    std::vector<ContactConstraint> constraints_;
};