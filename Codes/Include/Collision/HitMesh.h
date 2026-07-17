#pragma once
#include "./CollisionTypes.h"
#include "Geometry/PyramidMesh.h"
#include "Math/Matrix4x4.h"
#include "Math/Matrix3x3Physics.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include <vector>

namespace Atrum::Physics {

	inline Vector3 CalculateCentroid(const std::vector<Vector3>& vertices) {
		Vector3 sum = { 0.0f, 0.0f, 0.0f };
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
		std::vector<Vector3> localVertices{};

		// 辺
		std::vector<Edge> edges{};

		// 面
		std::vector<Face> faces{};

		//--------------------------------------------------------
		// ワールド空間
		//--------------------------------------------------------

		// ワールド座標
		std::vector<Vector3> worldVertices{};

		//--------------------------------------------------------
		// Transform
		//--------------------------------------------------------

		Vector3 position{};

		Quaternion rotation = Quaternion::Identity();

		Vector3 scale = { 1.0f,1.0f,1.0f };

		Matrix4x4 worldMatrix{};

		//--------------------------------------------------------
		// Physics
		//--------------------------------------------------------

		Vector3 velocity{};

		// ワールド空間角速度(rad/s)
		Vector3 angularVelocity{};

		Vector3 force{};

		Vector3 torque{};

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

			Vector3 rotateDelta = angularVelocity * deltaTime;

			float angle = VectorLength(rotateDelta);

			if (angle > 0.000001f) {

				Vector3 axis = rotateDelta / angle;

				Quaternion delta =
					Quaternion::FromAxisAngle(
						axis,
						angle);

				rotation =
					(delta * rotation).normalized();
			}

			UpdateMatrix();

		}

		//--------------------------------------------------------
		// 行列更新
		//--------------------------------------------------------

		void UpdateMatrix()
		{
			worldMatrix =
				MakeScaleMatrix(scale) *
				rotation.create_rotate_matrix() *
				MakeTranslateMatrix(position);

			UpdateWorldVertices();

			inverseInertiaTensorWorld = MakeWorldInverseInertiaTensor(
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
				worldVertices.push_back(
					VectorTransform(
					v,
					worldMatrix));
			}
		}

		//--------------------------------------------------------
		// Support Mapping
		//--------------------------------------------------------

		Vector3 GetSupportPoint(
			const Vector3& direction) const
		{
			float maxDot =
				-FLT_MAX;

			Vector3 best{};

			for (const auto& v : worldVertices)
			{
				float d =
					VectorDot(v, direction);

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

		Vector3 GetFaceNormal(uint32_t index) const {

			if (index >= faces.size()) {
				return {};
			}

			const Face& face = faces[index];

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

			const Vector3& v0 = worldVertices[i0];
			const Vector3& v1 = worldVertices[i1];
			const Vector3& v2 = worldVertices[i2];

			Vector3 edge0 = v1 - v0;
			Vector3 edge1 = v2 - v0;

			Vector3 normal =
				VectorCross(edge0, edge1);

			if (VectorLength(normal) <= 0.000001f) {
				return {};
			}

			return VectorNormalize(normal);
		}

		//--------------------------------------------------------
		// 面中心
		//--------------------------------------------------------

		Vector3 GetFaceCenter(
			uint32_t index) const
		{
			const Face& face =
				faces[index];

			Vector3 center{};

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

		Vector3 GetCenter() const
		{
			Vector3 center{};

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