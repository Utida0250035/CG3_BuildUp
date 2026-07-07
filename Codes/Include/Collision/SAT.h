#pragma once

#include <vector>

#include "HitMesh.h"
#include "CollisionTypes.h"

Projection ProjectVertices(
    const std::vector<Vector3>& vertices,
    const Vector3& axis);

float Overlap(
    const Projection& a,
    const Projection& b);

std::vector<Vector3> GetFaceAxes(
    const HitMesh& mesh);

std::vector<Vector3> GetEdgeAxes(
    const HitMesh& a,
    const HitMesh& b);

std::vector<Vector3> GetAxes(
    const HitMesh& a,
    const HitMesh& b);

std::vector<ContactPoint> GenerateContactPoints(
    const HitMesh& bodyA,
    const HitMesh& bodyB,
    const Vector3& normal,
    float depth);

SATResult TestSAT(
    const HitMesh& bodyA,
    const HitMesh& bodyB);