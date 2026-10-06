#pragma once

#include "./HitMesh.h"
#include "Geometry/Triangle.h"

namespace Atrum {

    class AssetModel;

}

namespace Atrum::Physics {

    class HitMeshBuilder {
    public:

        static HitMesh CreateFromPyramid(
            const Geometry::PyramidMesh& mesh);

        static HitMesh CreateFromObj(
            const AssetModel& model);

        static HitMesh CreateFromVertices(
            const std::vector<Math::Vector3>& vertices,
            const std::vector<Geometry::Edge>& edges,
            const std::vector<Geometry::Face>& faces);

        static HitMesh CreateFromTriangle(
            const Geometry::Triangle& triangle);


    };

}