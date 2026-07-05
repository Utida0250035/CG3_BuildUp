#pragma once

#include "CollisionTypes.h"
#include "HitMesh.h"

#include <cfloat>
#include <vector>

///----------------------------------------
/// 射影
///----------------------------------------

Projection ProjectVertices(
    const std::vector<Vector3>& vertices,
    const Vector3& axis);

///----------------------------------------
/// 射影の重なり量
///----------------------------------------

float Overlap(
    const Projection& a,
    const Projection& b);

///----------------------------------------
/// SAT軸生成
///----------------------------------------

std::vector<Vector3> GetFaceAxes(
    const HitMesh& mesh);

std::vector<Vector3> GetEdgeAxes(
    const HitMesh& meshA,
    const HitMesh& meshB);

std::vector<Vector3> GetAxes(
    const HitMesh& meshA,
    const HitMesh& meshB);

///----------------------------------------
/// SAT判定
///----------------------------------------

SATResult TestSAT(
    const HitMesh& meshA,
    const HitMesh& meshB);