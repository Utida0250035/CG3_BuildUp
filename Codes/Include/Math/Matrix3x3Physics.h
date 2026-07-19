#pragma once

#include "Engine/Alias/PhysicsAlias.h"

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include <vector>

namespace Atrum::Physics {

	struct Matrix3x3Physics {
		float m[3][3];

		inline constexpr Matrix3x3Physics operator+(
			const Matrix3x3Physics& other) const {

			Matrix3x3Physics result{};

			for (uint32_t y = 0; y < 3; ++y) {
				for (uint32_t x = 0; x < 3; ++x) {
					result.m[y][x] = m[y][x] + other.m[y][x];
				}
			}

			return result;
		}

		inline constexpr Matrix3x3Physics operator-(
			const Matrix3x3Physics& other) const {

			Matrix3x3Physics result{};

			for (uint32_t y = 0; y < 3; ++y) {
				for (uint32_t x = 0; x < 3; ++x) {
					result.m[y][x] = m[y][x] - other.m[y][x];
				}
			}

			return result;
		}

		inline constexpr Matrix3x3Physics operator*(
			const Matrix3x3Physics& other) const {

			Matrix3x3Physics result{};

			for (uint32_t y = 0; y < 3; ++y) {

				for (uint32_t x = 0; x < 3; ++x) {

					result.m[y][x] =
						m[y][0] * other.m[0][x] +
						m[y][1] * other.m[1][x] +
						m[y][2] * other.m[2][x];

				}

			}

			return result;
		}

		inline constexpr Matrix3x3Physics operator*(
			float scalar) const {

			Matrix3x3Physics result{};

			for (uint32_t y = 0; y < 3; ++y) {
				for (uint32_t x = 0; x < 3; ++x) {
					result.m[y][x] = m[y][x] * scalar;
				}
			}

			return result;
		}

		inline constexpr M::Vector3 operator*(
			const M::Vector3& vector) const {

			return {

				vector.x * m[0][0] +
				vector.y * m[1][0] +
				vector.z * m[2][0],

				vector.x * m[0][1] +
				vector.y * m[1][1] +
				vector.z * m[2][1],

				vector.x * m[0][2] +
				vector.y * m[1][2] +
				vector.z * m[2][2]

			};

		}

		inline Matrix3x3Physics& operator*=(
			const Matrix3x3Physics& other) {

			*this = *this * other;

			return *this;
		}

		inline Matrix3x3Physics& operator*=(
			float scalar) {

			for (uint32_t y = 0; y < 3; ++y) {
				for (uint32_t x = 0; x < 3; ++x) {
					m[y][x] *= scalar;
				}
			}

			return *this;
		}

		inline constexpr Matrix3x3Physics operator/(
			float scalar) const {

			Matrix3x3Physics result = *this;

			for (uint32_t y = 0; y < 3; ++y) {
				for (uint32_t x = 0; x < 3; ++x) {
					result.m[y][x] /= scalar;
				}
			}

			return result;

		}

		inline Matrix3x3Physics& operator/=(
			float scalar) {

			for (uint32_t y = 0; y < 3; ++y) {
				for (uint32_t x = 0; x < 3; ++x) {
					m[y][x] /= scalar;
				}
			}

			return *this;

		}

		inline constexpr bool operator==(
			const Matrix3x3Physics&) const = default;

		static Matrix3x3Physics Identity();

		static Matrix3x3Physics Zero();

		Matrix3x3Physics Transposed() const;

		M::Vector3 VectorTransform(
			const M::Vector3& vector) const;

		static Matrix3x3Physics FromQuaternion(
			const M::Quaternion& q);

		static Matrix3x3Physics BoxInverseInertiaTensor(
			float mass,
			float width,
			float height,
			float depth);

		static Matrix3x3Physics WorldInverseInertiaTensor(
			const Matrix3x3Physics& localInverseInertia,
			const M::Quaternion& rotation);

		Matrix3x3Physics Inversed() const;

		static Matrix3x3Physics VertexCloudInverseInertiaTensor(
			const std::vector<M::Vector3>& vertices,
			float mass);

	};

}