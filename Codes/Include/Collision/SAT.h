#pragma once

#include <vector>

#include "Collision/CollisionTypes.h"
#include "Collision/HitMesh.h"

namespace Atrum::Physics::SAT {

    Geometry::Projection ProjectVertices(
        const std::vector<Math::Vector3>& vertices,
        const Math::Vector3& axis);

    float Overlap(
        const Geometry::Projection& a,
        const Geometry::Projection& b);

    std::vector<Math::Vector3> GetFaceAxes(
        const HitMesh& mesh);

    std::vector<Math::Vector3> GetEdgeAxes(
        const HitMesh& a,
        const HitMesh& b);

    std::vector<Math::Vector3> GetAxes(
        const HitMesh& a,
        const HitMesh& b);

    std::vector<ContactPoint> GenerateContactPoints(
        const HitMesh& bodyA,
        const HitMesh& bodyB,
        const Math::Vector3& normal,
        float depth);

    SATResult TestSAT(
        const HitMesh& bodyA,
        const HitMesh& bodyB);

}