#include "Collision/HitMeshBuilder.h"
#include "Engine/Alias/PhysicsAlias.h"

#include <cassert>
#include <vector>

namespace {

	namespace P = ::Atrum::Physics;

	void SetupInertiaTensor(P::HitMesh& hitMesh) {

		if (hitMesh.inverseMass <= 0.0f || hitMesh.localVertices.empty()) {
			hitMesh.inverseInertiaTensorLocal = P::Matrix3x3Physics::Zero();
			hitMesh.inverseInertiaTensorWorld = P::Matrix3x3Physics::Zero();
			return;
		}

		hitMesh.inverseInertiaTensorLocal = P::Matrix3x3Physics::VertexCloudInverseInertiaTensor(
			hitMesh.localVertices,
			hitMesh.mass
		);

	}

}

namespace Atrum::Physics {

	HitMesh HitMeshBuilder::CreateFromPyramid(const G::PyramidMesh& mesh) {

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

		hitMesh.quaternion = M::Quaternion::Identity();

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
		const std::vector<M::Vector3>& vertices,
		const std::vector<G::Edge>& edges,
		const std::vector<G::Face>& faces) {

		HitMesh hitMesh{};

		hitMesh.localVertices = vertices;
		hitMesh.edges = edges;
		hitMesh.faces = faces;

		hitMesh.position = {};

		hitMesh.quaternion = M::Quaternion::Identity();

		hitMesh.scale = { 1.0f,1.0f,1.0f };

		hitMesh.velocity = {};

		hitMesh.angularVelocity = {};

		hitMesh.mass = 1.0f;
		hitMesh.inverseMass = 1.0f;

		hitMesh.restitution = 0.0f;
		hitMesh.friction = 0.8f;

		SetupInertiaTensor(hitMesh);

		hitMesh.UpdateMatrix();

		return hitMesh;

	}

	HitMesh HitMeshBuilder::CreateFromTriangle(const G::Triangle& triangle) {

		HitMesh hitMesh{};

		M::Vector3 normal = Cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0).Normalized();

		constexpr float thickness = 1.0f;
		M::Vector3 offset = normal * thickness;

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
			G::Face face{};
			face.indices = indices;

			hitMesh.faces.push_back(face);
		};

		AddFace({ 0, 1, 2 });       // 表
		AddFace({ 5, 4, 3 });       // 裏

		AddFace({ 0, 3, 4, 1 });    // 側面
		AddFace({ 1, 4, 5, 2 });    // 側面
		AddFace({ 2, 5, 3, 0 });    // 側面

		hitMesh.position = {};
		hitMesh.quaternion = M::Quaternion::Identity();
		hitMesh.scale = { 1.0f, 1.0f, 1.0f };

		hitMesh.velocity = {};
		hitMesh.angularVelocity = {};

		hitMesh.mass = 0.0f;
		hitMesh.inverseMass = 0.0f;

		hitMesh.restitution = 0.0f;
		hitMesh.friction = 0.8f;

		SetupInertiaTensor(hitMesh);

		hitMesh.UpdateMatrix();

		return hitMesh;
	}

}