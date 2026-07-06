#include "SAT.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace {

    constexpr float kAxisEpsilon = 0.000001f;
    constexpr float kContactTolerance = 0.05f;

    bool IsValidAxis(const Vector3& axis) {

        return VectorLength(axis) > kAxisEpsilon;
    }

    bool ContainsNearPoint(
        const std::vector<Vector3>& points,
        const Vector3& point,
        float epsilon) {

        for (const Vector3& p : points) {

            if (VectorLength(p - point) <= epsilon) {
                return true;
            }
        }

        return false;
    }

    void AddUniquePoint(
        std::vector<Vector3>& points,
        const Vector3& point,
        float epsilon) {

        if (ContainsNearPoint(points, point, epsilon)) {
            return;
        }

        points.push_back(point);
    }

    Vector3 CalculateAveragePoint(
        const std::vector<Vector3>& points) {

        if (points.empty()) {
            return {};
        }

        Vector3 result{};

        for (const Vector3& p : points) {
            result += p;
        }

        result /= static_cast<float>(points.size());

        return result;
    }

} // namespace

Projection ProjectVertices(
    const std::vector<Vector3>& vertices,
    const Vector3& axis) {

    Projection result{};

    if (vertices.empty()) {
        return result;
    }

    float first = VectorDot(vertices[0], axis);

    result.min = first;
    result.max = first;

    for (size_t i = 1; i < vertices.size(); ++i) {

        float projection = VectorDot(vertices[i], axis);

        result.min = std::min(result.min, projection);
        result.max = std::max(result.max, projection);
    }

    return result;
}

float Overlap(
    const Projection& a,
    const Projection& b) {

    return std::min(a.max, b.max) - std::max(a.min, b.min);
}

std::vector<Vector3> GetFaceAxes(
    const HitMesh& mesh) {

    std::vector<Vector3> axes;

    for (size_t i = 0; i < mesh.faces.size(); ++i) {

        Vector3 axis = mesh.GetFaceNormal(static_cast<int>(i));

        if (!IsValidAxis(axis)) {
            continue;
        }

        axis = VectorNormalize(axis);

        axes.push_back(axis);
    }

    return axes;
}

std::vector<Vector3> GetEdgeAxes(
    const HitMesh& a,
    const HitMesh& b) {

    std::vector<Vector3> axes;

    for (const Edge& edgeA : a.edges) {

        Vector3 a0 = a.worldVertices[edgeA.start];
        Vector3 a1 = a.worldVertices[edgeA.end];

        Vector3 dirA = a1 - a0;

        if (!IsValidAxis(dirA)) {
            continue;
        }

        dirA = VectorNormalize(dirA);

        for (const Edge& edgeB : b.edges) {

            Vector3 b0 = b.worldVertices[edgeB.start];
            Vector3 b1 = b.worldVertices[edgeB.end];

            Vector3 dirB = b1 - b0;

            if (!IsValidAxis(dirB)) {
                continue;
            }

            dirB = VectorNormalize(dirB);

            Vector3 axis = VectorCross(dirA, dirB);

            if (!IsValidAxis(axis)) {
                continue;
            }

            axis = VectorNormalize(axis);

            axes.push_back(axis);
        }
    }

    return axes;
}

std::vector<Vector3> GetAxes(
    const HitMesh& a,
    const HitMesh& b) {

    std::vector<Vector3> axes;

    std::vector<Vector3> faceAxesA = GetFaceAxes(a);
    std::vector<Vector3> faceAxesB = GetFaceAxes(b);
    std::vector<Vector3> edgeAxes = GetEdgeAxes(a, b);

    axes.insert(axes.end(), faceAxesA.begin(), faceAxesA.end());
    axes.insert(axes.end(), faceAxesB.begin(), faceAxesB.end());
    axes.insert(axes.end(), edgeAxes.begin(), edgeAxes.end());

    return axes;
}

std::vector<Vector3> GenerateContactPoints(
    const HitMesh& bodyA,
    const HitMesh& bodyB,
    const Vector3& normal,
    float depth) {

    std::vector<Vector3> contacts;

    if (!IsValidAxis(normal)) {
        return contacts;
    }

    Vector3 n = VectorNormalize(normal);

    Projection projectionA = ProjectVertices(bodyA.worldVertices, n);
    Projection projectionB = ProjectVertices(bodyB.worldVertices, n);

    float contactPlaneA = projectionA.max;
    float contactPlaneB = projectionB.min;

    float tolerance =
        std::max(kContactTolerance, depth * 0.5f);

    for (const Vector3& v : bodyA.worldVertices) {

        float d = std::abs(VectorDot(v, n) - contactPlaneA);

        if (d <= tolerance) {
            AddUniquePoint(contacts, v, 0.001f);
        }
    }

    for (const Vector3& v : bodyB.worldVertices) {

        float d = std::abs(VectorDot(v, n) - contactPlaneB);

        if (d <= tolerance) {
            AddUniquePoint(contacts, v, 0.001f);
        }
    }

    if (contacts.empty()) {

        Vector3 supportA = bodyA.GetSupportPoint(n);
        Vector3 supportB = bodyB.GetSupportPoint(-n);

        AddUniquePoint(contacts, (supportA + supportB) * 0.5f, 0.001f);
    }

    constexpr size_t kMaxContactCount = 2;

    if (contacts.size() > kMaxContactCount) {

        Vector3 average = CalculateAveragePoint(contacts);

        std::sort(
            contacts.begin(),
            contacts.end(),
            [average](const Vector3& lhs, const Vector3& rhs) {

            float dl = VectorLength(lhs - average);
            float dr = VectorLength(rhs - average);

            return dl > dr;
        });

        contacts.resize(kMaxContactCount);
    }

    return contacts;
}

SATResult TestSAT(
    const HitMesh& bodyA,
    const HitMesh& bodyB) {

    SATResult result{};

    std::vector<Vector3> axes = GetAxes(bodyA, bodyB);

    if (axes.empty()) {
        return result;
    }

    float minOverlap = FLT_MAX;
    Vector3 minAxis{};

    for (Vector3 axis : axes) {

        if (!IsValidAxis(axis)) {
            continue;
        }

        axis = VectorNormalize(axis);

        Projection projectionA =
            ProjectVertices(bodyA.worldVertices, axis);

        Projection projectionB =
            ProjectVertices(bodyB.worldVertices, axis);

        float overlap =
            Overlap(projectionA, projectionB);

        if (overlap <= 0.0f) {
            result.hit = false;
            return result;
        }

        if (overlap < minOverlap) {

            minOverlap = overlap;
            minAxis = axis;
        }
    }

    if (!IsValidAxis(minAxis)) {
        result.hit = false;
        return result;
    }

    Vector3 centerA = bodyA.GetCenter();
    Vector3 centerB = bodyB.GetCenter();

    Vector3 centerDirection = centerB - centerA;

    if (VectorDot(centerDirection, minAxis) < 0.0f) {
        minAxis = -minAxis;
    }

    result.hit = true;
    result.normal = VectorNormalize(minAxis);
    result.depth = minOverlap;

    result.contactPoints =
        GenerateContactPoints(
            bodyA,
            bodyB,
            result.normal,
            result.depth);

    if (!result.contactPoints.empty()) {
        result.contactPoint = CalculateAveragePoint(result.contactPoints);
    } else {

        Vector3 supportA = bodyA.GetSupportPoint(result.normal);
        Vector3 supportB = bodyB.GetSupportPoint(-result.normal);

        result.contactPoint = (supportA + supportB) * 0.5f;
        result.contactPoints.push_back(result.contactPoint);
    }

    return result;
}