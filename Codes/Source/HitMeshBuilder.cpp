#include "HitMeshBuilder.h"

#include <cassert>

namespace {

    void SetupInertiaTensor(HitMesh& hitMesh) {

        if (hitMesh.inverseMass <= 0.0f || hitMesh.localVertices.empty()) {
            hitMesh.inverseInertiaTensorLocal = MakeZeroMatrix3x3Physics();
            hitMesh.inverseInertiaTensorWorld = MakeZeroMatrix3x3Physics();
            return;
        }

        hitMesh.inverseInertiaTensorLocal =
            MakeVertexCloudInverseInertiaTensor(
                hitMesh.localVertices,
                hitMesh.mass);
    }

}

HitMesh HitMeshBuilder::CreateFromPyramid(const PyramidMesh& mesh) {

    HitMesh hitMesh{};

    //----------------------------------------------------------
    // 頂点, 辺, 面
    //----------------------------------------------------------

    hitMesh.localVertices = mesh.collisionVertices;

    hitMesh.edges = mesh.edges;

    hitMesh.faces = mesh.faces;

    //----------------------------------------------------------
    // 初期Transform
    //----------------------------------------------------------

    hitMesh.position = {};

    hitMesh.rotation = Quaternion::Identity();

    hitMesh.scale = { 1.0f,1.0f,1.0f };

    hitMesh.velocity = {};

    hitMesh.angularVelocity = {};

    hitMesh.mass = 1.0f;
    hitMesh.inverseMass = 1.0f;

    hitMesh.restitution = 0.0f;
    hitMesh.friction = 0.8f;

    //----------------------------------------------------------
    // 物理
    //----------------------------------------------------------

    SetupInertiaTensor(hitMesh);

    hitMesh.UpdateMatrix();

    return hitMesh;

}

HitMesh HitMeshBuilder::CreateFromObj(const AssetModel& model) {

    model;

    HitMesh hitMesh{};

    //----------------------------------------------------------
    // TODO
    // OBJLoader完成後に実装
    //----------------------------------------------------------
    

    SetupInertiaTensor(hitMesh);

    hitMesh.UpdateMatrix();

    return hitMesh;

}

HitMesh HitMeshBuilder::CreateFromVertices(
    const std::vector<Vector3>& vertices,
    const std::vector<Edge>& edges,
    const std::vector<Face>& faces) {

    HitMesh hitMesh{};

    hitMesh.localVertices = vertices;
    hitMesh.edges = edges;
    hitMesh.faces = faces;

    hitMesh.position = {};

    hitMesh.rotation = Quaternion::Identity();

    hitMesh.scale = { 1.0f,1.0f,1.0f };

    hitMesh.velocity = {};

    hitMesh.angularVelocity = {};

    hitMesh.mass = 1.0f;
    hitMesh.inverseMass = 1.0f;

    hitMesh.restitution = 0.3f;
    hitMesh.friction = 2.8f;

    SetupInertiaTensor(hitMesh);

    hitMesh.UpdateMatrix();

    return hitMesh;

}

HitMesh HitMeshBuilder::CreateFromTriangle(const Triangle& triangle) {

    HitMesh hitMesh{};

    Vector3 normal = VectorNormalize(
        VectorCross(
        triangle.v1 - triangle.v0,
        triangle.v2 - triangle.v0));

    constexpr float thickness = 1.0f;
    Vector3 offset = normal * thickness;

    hitMesh.localVertices = {
        triangle.v0 + offset,
        triangle.v1 + offset,
        triangle.v2 + offset,

        triangle.v0 - offset,
        triangle.v1 - offset,
        triangle.v2 - offset
    };

    hitMesh.edges = {
        {0,1}, {1,2}, {2,0},
        {3,4}, {4,5}, {5,3},
        {0,3}, {1,4}, {2,5}
    };

    hitMesh.faces.clear();

    auto AddFace = [&](std::initializer_list<uint32_t> indices) {
        Face face{};
        face.indices = indices;

        const Vector3& v0 = hitMesh.localVertices[face.indices[0]];
        const Vector3& v1 = hitMesh.localVertices[face.indices[1]];
        const Vector3& v2 = hitMesh.localVertices[face.indices[2]];

        face.normal = VectorNormalize(VectorCross(v1 - v0, v2 - v0));

        hitMesh.faces.push_back(face);
    };

    AddFace({ 0, 1, 2 });       // 表
    AddFace({ 5, 4, 3 });       // 裏

    AddFace({ 0, 3, 4, 1 });    // 側面
    AddFace({ 1, 4, 5, 2 });    // 側面
    AddFace({ 2, 5, 3, 0 });    // 側面

    hitMesh.position = {};
    hitMesh.rotation = Quaternion::Identity();
    hitMesh.scale = { 1.0f, 1.0f, 1.0f };

    hitMesh.velocity = {};
    hitMesh.angularVelocity = {};

    hitMesh.mass = 0.0f;
    hitMesh.inverseMass = 0.0f;

    hitMesh.restitution = 0.0f;
    hitMesh.friction = 2.8f;

    SetupInertiaTensor(hitMesh);

    hitMesh.UpdateMatrix();

    return hitMesh;
}