#pragma once

#include "HitMesh.h"
#include "Triangle.h"

class AssetModel;

class HitMeshBuilder {
public:

    static HitMesh CreateFromPyramid(
        const PyramidMesh& mesh);

    static HitMesh CreateFromObj(
        const AssetModel& model);

    static HitMesh CreateFromVertices(
        const std::vector<Vector3>& vertices,
        const std::vector<Edge>& edges,
        const std::vector<Face>& faces);

    static HitMesh CreateFromTriangle(
        const Triangle& triangle);


};