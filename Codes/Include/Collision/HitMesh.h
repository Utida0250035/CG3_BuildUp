#pragma once
#include "Collision/CollisionTypes.h"
#include "Geometry/PyramidMesh.h"
#include "Math/Matrix4x4.h"
#include "Math/Matrix3x3Physics.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include <vector>

#include "Engine/Alias/PhysicsAlias.h"

namespace Atrum::Physics {

	inline Math::Vector3 CalculateCentroid(const std::vector<Math::Vector3>& vertices) {
		Math::Vector3 sum = { 0.0f, 0.0f, 0.0f };
		for (const auto& v : vertices) {
			sum.x += v.x;
			sum.y += v.y;
			sum.z += v.z;
		}
		float count = static_cast<float>(vertices.size());
		return { sum.x / count, sum.y / count, sum.z / count };
	}

	///------------------------------------------------------------
	/// 凸メッシュ
	/// SAT・GJK・EPA・剛体計算で使用する
	///------------------------------------------------------------
	struct HitMesh {

		//--------------------------------------------------------
		// メッシュ情報（ローカル空間）
		//--------------------------------------------------------

		// 頂点
		std::vector<Math::Vector3> localVertices{};

		// 辺
		std::vector<Geometry::Edge> edges{};

		// 面
		std::vector<G::Face> faces{};

		//--------------------------------------------------------
		// ワールド空間
		//--------------------------------------------------------

		// ワールド座標
		std::vector<M::Vector3> worldVertices{};

		//--------------------------------------------------------
		// Transform
		//--------------------------------------------------------

		M::Vector3 position{};

		M::Quaternion rotation = M::Quaternion::Identity();

		M::Vector3 scale = { 1.0f,1.0f,1.0f };

		M::Matrix4x4 worldMatrix{};

		//--------------------------------------------------------
		// Physics
		//--------------------------------------------------------

		M::Vector3 velocity{};

		// ワールド空間角速度(rad/s)
		M::Vector3 angularVelocity{};

		M::Vector3 force{};

		M::Vector3 torque{};

		Matrix3x3Physics inertiaTensor{};

		Matrix3x3Physics inverseInertiaTensor{};

		Matrix3x3Physics inverseInertiaTensorLocal{};

		Matrix3x3Physics inverseInertiaTensorWorld{};

		float mass = 1.0f;

		float inverseMass = 1.0f;

		float restitution = 0.3f;

		float friction = 0.5f;

		bool useGravity = true;

		bool isStatic = false;

		bool isSleeping = false;
		float sleepTimer = 0.0f;

		//--------------------------------------------------------
		// 更新
		//--------------------------------------------------------

		void Update(float deltaTime)
		{
			//--------------------
			// 平行移動
			//--------------------

			position += velocity * deltaTime;

			//--------------------
			// 回転
			//--------------------

			M::Vector3 rotateDelta = angularVelocity * deltaTime;

			float angle = rotateDelta.Length();

			if (angle > 0.000001f) {

				M::Vector3 axis = rotateDelta / angle;

				M::Quaternion delta =
					M::Quaternion::FromAxisAngle(
						axis,
						angle);

				rotation =
					(delta * rotation).Normalized();
			}

			UpdateMatrix();

		}

		//--------------------------------------------------------
		// 行列更新
		//--------------------------------------------------------

		void UpdateMatrix()
		{
			worldMatrix =
				M::Matrix4x4::Scale(scale) *
				rotation.MakeRotateMatrix() *
				M::Matrix4x4::Translate(position);

			UpdateWorldVertices();

			inverseInertiaTensorWorld = Matrix3x3Physics::WorldInverseInertiaTensor(
				inverseInertiaTensorLocal,
				rotation
			);

		}

		//--------------------------------------------------------
		// ワールド頂点更新
		//--------------------------------------------------------

		void UpdateWorldVertices()
		{
			worldVertices.clear();

			worldVertices.reserve(localVertices.size());

			for (const auto& v : localVertices)
			{
				worldVertices.push_back(worldMatrix.Transform(v));
			}
		}

		//--------------------------------------------------------
		// Support Mapping
		//--------------------------------------------------------

		M::Vector3 GetSupportPoint(
			const M::Vector3& direction) const
		{
			float maxDot =
				-FLT_MAX;

			M::Vector3 best{};

			for (const auto& v : worldVertices)
			{
				float d = v.Dot(direction);

				if (d > maxDot)
				{
					maxDot = d;
					best = v;
				}
			}

			return best;
		}

		//--------------------------------------------------------
		// 面法線取得
		//--------------------------------------------------------

		M::Vector3 GetFaceNormal(uint32_t index) const {

			if (index >= faces.size()) {
				return {};
			}

			const G::Face& face = faces[index];

			if (face.indices.size() < 3) {
				return {};
			}

			uint32_t i0 = face.indices[0];
			uint32_t i1 = face.indices[1];
			uint32_t i2 = face.indices[2];

			if (i0 >= worldVertices.size() ||
				i1 >= worldVertices.size() ||
				i2 >= worldVertices.size()) {
				return {};
			}

			const M::Vector3& v0 = worldVertices[i0];
			const M::Vector3& v1 = worldVertices[i1];
			const M::Vector3& v2 = worldVertices[i2];

			M::Vector3 edge0 = v1 - v0;
			M::Vector3 edge1 = v2 - v0;

			M::Vector3 normal = edge0.Cross(edge1);

			if (normal.Length() <= 0.000001f) {
				return {};
			}

			return normal.Normalized();
		}

		//--------------------------------------------------------
		// 面中心
		//--------------------------------------------------------

		M::Vector3 GetFaceCenter(
			uint32_t index) const
		{
			const G::Face& face =
				faces[index];

			M::Vector3 center{};

			for (uint32_t i : face.indices)
			{
				center += worldVertices[i];
			}

			center /=
				static_cast<float>(
					face.indices.size());

			return center;
		}

		//--------------------------------------------------------
		// メッシュ重心
		//--------------------------------------------------------

		M::Vector3 GetCenter() const
		{
			M::Vector3 center{};

			for (const auto& v : worldVertices)
			{
				center += v;
			}

			center /=
				static_cast<float>(
					worldVertices.size());

			return center;
		}

	};

}