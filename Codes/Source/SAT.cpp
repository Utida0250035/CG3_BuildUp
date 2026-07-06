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
        const std::vector<ContactPoint>& contacts,
        const Vector3& point,
        float epsilon) {

        for (const ContactPoint& contact : contacts) {

            if (VectorLength(contact.position - point) <= epsilon) {
                return true;
            }
        }

        return false;
    }

    void AddUniquePoint(
        std::vector<ContactPoint>& contacts,
        const Vector3& point,
        float penetration,
        float epsilon) {

        if (ContainsNearPoint(contacts, point, epsilon)) {
            return;
        }

        ContactPoint contact{};
        contact.position = point;
        contact.penetration = penetration;

        contacts.push_back(contact);
    }

    Vector3 CalculateAveragePoint(
        const std::vector<ContactPoint>& contacts) {

        if (contacts.empty()) {
            return {};
        }

        Vector3 result{};

        for (const ContactPoint& contact : contacts) {
            result += contact.position;
        }

        result /= static_cast<float>(contacts.size());

        return result;
    }

    struct FaceQuery {
        int index = -1;
        Vector3 normal{};
        Vector3 center{};
        float dot = -FLT_MAX;
    };

    struct ReferenceFace {
        const HitMesh* mesh = nullptr;
        int faceIndex = -1;
        Vector3 normal{};
        Vector3 center{};
        bool referenceIsA = true;
    };

    Vector3 CalculateFaceCenter(
        const HitMesh& mesh,
        const Face& face) {

        Vector3 center{};

        if (face.indices.empty()) {
            return center;
        }

        for (uint32_t index : face.indices) {

            if (index >= mesh.worldVertices.size()) {
                continue;
            }

            center += mesh.worldVertices[index];
        }

        center /= static_cast<float>(face.indices.size());

        return center;
    }

    Vector3 CalculateOutwardFaceNormal(
        const HitMesh& mesh,
        const Face& face) {

        if (face.indices.size() < 3) {
            return {};
        }

        uint32_t i0 = face.indices[0];
        uint32_t i1 = face.indices[1];
        uint32_t i2 = face.indices[2];

        if (i0 >= mesh.worldVertices.size() ||
            i1 >= mesh.worldVertices.size() ||
            i2 >= mesh.worldVertices.size()) {
            return {};
        }

        const Vector3& v0 = mesh.worldVertices[i0];
        const Vector3& v1 = mesh.worldVertices[i1];
        const Vector3& v2 = mesh.worldVertices[i2];

        Vector3 normal =
            VectorCross(v1 - v0, v2 - v0);

        if (!IsValidAxis(normal)) {
            return {};
        }

        normal = VectorNormalize(normal);

        Vector3 faceCenter =
            CalculateFaceCenter(mesh, face);

        Vector3 meshCenter =
            mesh.GetCenter();

        Vector3 outwardDirection =
            faceCenter - meshCenter;

        if (VectorDot(normal, outwardDirection) < 0.0f) {
            normal = -normal;
        }

        return normal;
    }

    FaceQuery FindMostAlignedFace(
        const HitMesh& mesh,
        const Vector3& direction) {

        FaceQuery result{};

        Vector3 dir = VectorNormalize(direction);

        for (size_t i = 0; i < mesh.faces.size(); ++i) {

            const Face& face = mesh.faces[i];

            Vector3 normal =
                CalculateOutwardFaceNormal(mesh, face);

            if (!IsValidAxis(normal)) {
                continue;
            }

            float d =
                VectorDot(normal, dir);

            if (d > result.dot) {

                result.index = static_cast<int>(i);
                result.normal = normal;
                result.center = CalculateFaceCenter(mesh, face);
                result.dot = d;
            }
        }

        return result;
    }

    FaceQuery FindMostAntiParallelFace(
        const HitMesh& mesh,
        const Vector3& referenceNormal) {

        FaceQuery result{};
        result.dot = FLT_MAX;

        Vector3 refN =
            VectorNormalize(referenceNormal);

        for (size_t i = 0; i < mesh.faces.size(); ++i) {

            const Face& face = mesh.faces[i];

            Vector3 normal =
                CalculateOutwardFaceNormal(mesh, face);

            if (!IsValidAxis(normal)) {
                continue;
            }

            float d =
                VectorDot(normal, refN);

            if (d < result.dot) {

                result.index = static_cast<int>(i);
                result.normal = normal;
                result.center = CalculateFaceCenter(mesh, face);
                result.dot = d;
            }
        }

        return result;
    }

    std::vector<Vector3> GetFaceVertices(
        const HitMesh& mesh,
        const Face& face) {

        std::vector<Vector3> vertices;

        for (uint32_t index : face.indices) {

            if (index >= mesh.worldVertices.size()) {
                continue;
            }

            vertices.push_back(mesh.worldVertices[index]);
        }

        return vertices;
    }

    std::vector<Vector3> ClipPolygonByPlane(
        const std::vector<Vector3>& polygon,
        const Vector3& planePoint,
        const Vector3& planeNormal) {

        std::vector<Vector3> result;

        if (polygon.empty()) {
            return result;
        }

        constexpr float kPlaneEpsilon = 0.0001f;

        for (size_t i = 0; i < polygon.size(); ++i) {

            const Vector3& current =
                polygon[i];

            const Vector3& next =
                polygon[(i + 1) % polygon.size()];

            float currentDistance =
                VectorDot(current - planePoint, planeNormal);

            float nextDistance =
                VectorDot(next - planePoint, planeNormal);

            bool currentInside =
                currentDistance <= kPlaneEpsilon;

            bool nextInside =
                nextDistance <= kPlaneEpsilon;

            if (currentInside && nextInside) {

                result.push_back(next);

            } else if (currentInside && !nextInside) {

                float t =
                    currentDistance /
                    (currentDistance - nextDistance);

                Vector3 intersection =
                    current + (next - current) * t;

                result.push_back(intersection);

            } else if (!currentInside && nextInside) {

                float t =
                    currentDistance /
                    (currentDistance - nextDistance);

                Vector3 intersection =
                    current + (next - current) * t;

                result.push_back(intersection);
                result.push_back(next);
            }
        }

        return result;
    }

    std::vector<Vector3> ClipIncidentFaceByReferenceFace(
        const HitMesh& referenceMesh,
        const Face& referenceFace,
        const Vector3& referenceNormal,
        const std::vector<Vector3>& incidentPolygon) {

        std::vector<Vector3> clipped =
            incidentPolygon;

        if (referenceFace.indices.size() < 3) {
            return clipped;
        }

        Vector3 referenceCenter =
            CalculateFaceCenter(referenceMesh, referenceFace);

        for (size_t i = 0; i < referenceFace.indices.size(); ++i) {

            uint32_t index0 =
                referenceFace.indices[i];

            uint32_t index1 =
                referenceFace.indices[(i + 1) % referenceFace.indices.size()];

            if (index0 >= referenceMesh.worldVertices.size() ||
                index1 >= referenceMesh.worldVertices.size()) {
                continue;
            }

            Vector3 v0 =
                referenceMesh.worldVertices[index0];

            Vector3 v1 =
                referenceMesh.worldVertices[index1];

            Vector3 edge =
                v1 - v0;

            if (!IsValidAxis(edge)) {
                continue;
            }

            Vector3 sideNormal =
                VectorCross(edge, referenceNormal);

            if (!IsValidAxis(sideNormal)) {
                continue;
            }

            sideNormal =
                VectorNormalize(sideNormal);

            float centerSide =
                VectorDot(referenceCenter - v0, sideNormal);

            if (centerSide > 0.0f) {
                sideNormal = -sideNormal;
            }

            clipped =
                ClipPolygonByPlane(
                    clipped,
                    v0,
                    sideNormal);

            if (clipped.empty()) {
                break;
            }
        }

        return clipped;
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

std::vector<ContactPoint> GenerateContactPoints(
    const HitMesh& bodyA,
    const HitMesh& bodyB,
    const Vector3& normal,
    float depth) {

    std::vector<ContactPoint> contacts;

    if (!IsValidAxis(normal)) {
        return contacts;
    }

    Vector3 n =
        VectorNormalize(normal);

    FaceQuery faceA =
        FindMostAlignedFace(bodyA, n);

    FaceQuery faceB =
        FindMostAlignedFace(bodyB, -n);

    if (faceA.index < 0 || faceB.index < 0) {
        return contacts;
    }

    ReferenceFace reference{};

    const HitMesh* incidentMesh = nullptr;
    int incidentFaceIndex = -1;

    if (faceA.dot >= faceB.dot) {

        reference.mesh = &bodyA;
        reference.faceIndex = faceA.index;
        reference.normal = faceA.normal;
        reference.center = faceA.center;
        reference.referenceIsA = true;

        incidentMesh = &bodyB;

        FaceQuery incidentFace =
            FindMostAntiParallelFace(
                bodyB,
                reference.normal);

        incidentFaceIndex = incidentFace.index;

    } else {

        reference.mesh = &bodyB;
        reference.faceIndex = faceB.index;
        reference.normal = faceB.normal;
        reference.center = faceB.center;
        reference.referenceIsA = false;

        incidentMesh = &bodyA;

        FaceQuery incidentFace =
            FindMostAntiParallelFace(
                bodyA,
                reference.normal);

        incidentFaceIndex = incidentFace.index;
    }

    if (reference.mesh == nullptr ||
        incidentMesh == nullptr ||
        reference.faceIndex < 0 ||
        incidentFaceIndex < 0) {
        return contacts;
    }

    const Face& referenceFace =
        reference.mesh->faces[reference.faceIndex];

    const Face& incidentFace =
        incidentMesh->faces[incidentFaceIndex];

    std::vector<Vector3> incidentPolygon =
        GetFaceVertices(*incidentMesh, incidentFace);

    if (incidentPolygon.empty()) {
        return contacts;
    }

    std::vector<Vector3> clipped =
        ClipIncidentFaceByReferenceFace(
            *reference.mesh,
            referenceFace,
            reference.normal,
            incidentPolygon);

    if (clipped.empty()) {
        return contacts;
    }

    constexpr float kContactSlop = 0.01f;

    float referencePlane =
        VectorDot(reference.center, reference.normal);

    for (const Vector3& point : clipped) {

        float distance =
            VectorDot(point, reference.normal) - referencePlane;

        if (distance <= kContactSlop + depth) {

            float pointPenetration =
                std::max(-distance, 0.0f);

            if (pointPenetration <= 0.0f) {
                continue;
            }

            Vector3 projectedPoint =
                point - reference.normal * distance;

            AddUniquePoint(
                contacts,
                projectedPoint,
                pointPenetration,
                0.001f);
        }
    }

    constexpr size_t kMaxContactCount = 4;

    if (contacts.size() > kMaxContactCount) {

        Vector3 average =
            CalculateAveragePoint(contacts);

        std::sort(
            contacts.begin(),
            contacts.end(),
            [average](const ContactPoint& lhs, const ContactPoint& rhs) {

            float dl =
                VectorLength(lhs.position - average);

            float dr =
                VectorLength(rhs.position - average);

            return dl > dr;
        });

        contacts.resize(kMaxContactCount);
    }

    if (contacts.empty()) {

        Vector3 supportA =
            bodyA.GetSupportPoint(n);

        Vector3 supportB =
            bodyB.GetSupportPoint(-n);

        AddUniquePoint(
            contacts,
            (supportA + supportB) * 0.5f,
            depth,
            0.001f);
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

    result.contactPoints =
        GenerateContactPoints(
            bodyA,
            bodyB,
            result.normal,
            minOverlap);

    return result;
}