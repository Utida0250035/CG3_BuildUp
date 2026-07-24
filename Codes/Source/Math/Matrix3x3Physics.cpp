#include "Math/Matrix3x3Physics.h"
#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <vector>

namespace Atrum::Physics {

	Matrix3x3Physics Matrix3x3Physics::Identity() {

		Matrix3x3Physics result{};

		result.m[0][0] = 1.0f;
		result.m[1][1] = 1.0f;
		result.m[2][2] = 1.0f;

		return result;
	}

	Matrix3x3Physics Matrix3x3Physics::Zero() {

		return Matrix3x3Physics{};

	}

	Matrix3x3Physics Matrix3x3Physics::Transposed() const {

		Matrix3x3Physics result{};

		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				result.m[y][x] = m[x][y];
			}
		}

		return result;
	}

	M::Vector3 Matrix3x3Physics::VectorTransform(
		const M::Vector3& vector) const {

		return M::Vector3{
			vector.x * m[0][0] + vector.y * m[1][0] + vector.z * m[2][0],
			vector.x * m[0][1] + vector.y * m[1][1] + vector.z * m[2][1],
			vector.x * m[0][2] + vector.y * m[1][2] + vector.z * m[2][2]
		};
	}

	Matrix3x3Physics Matrix3x3Physics::FromQuaternion(
		const M::Quaternion& q) {

		M::Quaternion nq = q.Normalized();

		float x = nq.x;
		float y = nq.y;
		float z = nq.z;
		float w = nq.w;

		Matrix3x3Physics result{};

		result.m[0][0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
		result.m[0][1] = 2.0f * x * y - 2.0f * w * z;
		result.m[0][2] = 2.0f * x * z + 2.0f * w * y;

		result.m[1][0] = 2.0f * x * y + 2.0f * w * z;
		result.m[1][1] = 1.0f - 2.0f * x * x - 2.0f * z * z;
		result.m[1][2] = 2.0f * y * z - 2.0f * w * x;

		result.m[2][0] = 2.0f * x * z - 2.0f * w * y;
		result.m[2][1] = 2.0f * y * z + 2.0f * w * x;
		result.m[2][2] = 1.0f - 2.0f * x * x - 2.0f * y * y;

		return result;
	}

	Matrix3x3Physics Matrix3x3Physics::BoxInverseInertiaTensor(
		float mass,
		float width,
		float height,
		float depth) {

#ifdef _DEBUG

		std::cout << std::format(
			"mass={} width={} height={} depth={}",
			mass,
			width,
			height,
			depth)
			<< std::endl;

#endif



		if (mass <= 0.0f) {
			return Zero();
		}

		float ix = mass * (height * height + depth * depth) / 12.0f;
		float iy = mass * (width * width + depth * depth) / 12.0f;
		float iz = mass * (width * width + height * height) / 12.0f;

		Matrix3x3Physics result{};

		if (ix > 0.000001f) {
			result.m[0][0] = 1.0f / ix;
		}

		if (iy > 0.000001f) {
			result.m[1][1] = 1.0f / iy;
		}

		if (iz > 0.000001f) {
			result.m[2][2] = 1.0f / iz;
		}

		return result;
	}

	Matrix3x3Physics Matrix3x3Physics::WorldInverseInertiaTensor(
		const Matrix3x3Physics& localInverseInertia,
		const M::Quaternion& rotation) {

		Matrix3x3Physics r = FromQuaternion(rotation.Normalized());

		Matrix3x3Physics rt = r.Transposed();

		return r * localInverseInertia * rt;
	}

	Matrix3x3Physics Matrix3x3Physics::Inversed() const {

		const float a = m[0][0];
		const float b = m[0][1];
		const float c = m[0][2];

		const float d = m[1][0];
		const float e = m[1][1];
		const float f = m[1][2];

		const float g = m[2][0];
		const float h = m[2][1];
		const float i = m[2][2];

		const float det =
			a * (e * i - f * h) -
			b * (d * i - f * g) +
			c * (d * h - e * g);

		if (std::abs(det) < 0.000001f) {
			return Zero();
		}

		const float invDet = 1.0f / det;

		Matrix3x3Physics result{};

		result.m[0][0] = (e * i - f * h) * invDet;
		result.m[0][1] = -(b * i - c * h) * invDet;
		result.m[0][2] = (b * f - c * e) * invDet;

		result.m[1][0] = -(d * i - f * g) * invDet;
		result.m[1][1] = (a * i - c * g) * invDet;
		result.m[1][2] = -(a * f - c * d) * invDet;

		result.m[2][0] = (d * h - e * g) * invDet;
		result.m[2][1] = -(a * h - b * g) * invDet;
		result.m[2][2] = (a * e - b * d) * invDet;

		return result;
	}

	Matrix3x3Physics Matrix3x3Physics::VertexCloudInverseInertiaTensor(
		const std::vector<M::Vector3>& vertices,
		float mass) {

		if (mass <= 0.0f || vertices.empty()) {
			return Zero();
		}

		Matrix3x3Physics inertia{};

		const float pointMass =
			mass / static_cast<float>(vertices.size());

		for (const M::Vector3& r : vertices) {

			const float x = r.x;
			const float y = r.y;
			const float z = r.z;

			const float xx = x * x;
			const float yy = y * y;
			const float zz = z * z;

			inertia.m[0][0] += pointMass * (yy + zz);
			inertia.m[1][1] += pointMass * (xx + zz);
			inertia.m[2][2] += pointMass * (xx + yy);

			inertia.m[0][1] -= pointMass * x * y;
			inertia.m[1][0] -= pointMass * x * y;

			inertia.m[0][2] -= pointMass * x * z;
			inertia.m[2][0] -= pointMass * x * z;

			inertia.m[1][2] -= pointMass * y * z;
			inertia.m[2][1] -= pointMass * y * z;
		}

		constexpr float kInertiaScale = 5.0f;

		inertia *= kInertiaScale;

		return inertia.Inversed();
	}

}