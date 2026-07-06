#pragma once

#include "CollisionTypes.h"
#include "HitMesh.h"
#include "SAT.h"

#include <vector>

class CollisionManager {
public:

    void Clear();

    void AddBody(HitMesh* body);

    void CheckCollision();

private:

    void ResolveCollision(
        HitMesh& meshA,
        HitMesh& meshB,
        const SATResult& result
    );

    void ResolvePosition(
        HitMesh& meshA,
        HitMesh& meshB,
        const SATResult& result
    );

    void ResolveVelocity(
        HitMesh& meshA,
        HitMesh& meshB,
        const SATResult& result
    );

    bool ShouldApplyImpactImpulse(
        HitMesh& meshA,
        HitMesh& meshB,
        const SATResult& result
    );

private:

    std::vector<HitMesh*> bodies_{};

};