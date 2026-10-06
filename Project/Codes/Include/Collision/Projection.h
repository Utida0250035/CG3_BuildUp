#pragma once

#include "Geometry/Triangle.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include <vector>


struct Projection {

    float min;
    float max;
};

inline constexpr Projection ProjectVertices(
    const std::vector<Vector3>& vertices,
    const Matrix4x4& world,
    const Vector3& axis)
{
    Projection p{ FLT_MAX,-FLT_MAX };

    for (const auto& v : vertices)
    {
        Vector3 w = VectorTransform(v, world);
        float d = VectorDot(w, axis);

        p.min = std::min(p.min, d);
        p.max = std::max(p.max, d);
    }

    return p;
}

inline constexpr Projection ProjectTriangle(
    const Triangle& tri,
    const Vector3& axis)
{
    Projection p{ FLT_MAX,-FLT_MAX };

    float d0 = VectorDot(tri.v0, axis);
    float d1 = VectorDot(tri.v1, axis);
    float d2 = VectorDot(tri.v2, axis);

    p.min = std::min({ d0,d1,d2 });
    p.max = std::max({ d0,d1,d2 });

    return p;
}

inline constexpr float GetOverlap(
    const Projection& a,
    const Projection& b)
{
    return std::min(a.max, b.max)
        - std::max(a.min, b.min);
}