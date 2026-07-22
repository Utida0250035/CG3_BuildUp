#pragma once

#include "Vector3.h"
#include <cassert>

namespace Atrum::Math {

	struct Quaternion;

	struct Matrix4x4 {
		float m[4][4]{};

		constexpr Matrix4x4& operator+=(const Matrix4x4& other) {

			for (size_t i = 0; i < 4; ++i) {

				for (size_t j = 0; j < 4; ++j) {

					m[i][j] += other.m[i][j];

				}

			}

			return (*this);

		}

		constexpr Matrix4x4 operator+(const Matrix4x4& other) {

			Matrix4x4 result = (*this);
			result += other;

			return result;

		}

		constexpr Matrix4x4& operator-=(const Matrix4x4& other) {

			for (size_t i = 0; i < 4; ++i) {

				for (size_t j = 0; j < 4; ++j) {

					m[i][j] -= other.m[i][j];

				}

			}

			return (*this);

		}

		constexpr Matrix4x4 operator-(const Matrix4x4& other) {
			Matrix4x4 result = (*this);
			result -= other;

			return result;

		}

		constexpr Matrix4x4 operator*(const Matrix4x4& other) {

			Matrix4x4 result = {};

			for (size_t i = 0; i < 4; i++) {

				for (size_t j = 0; j < 4; j++) {

					for (size_t k = 0; k < 4; k++) {

						result.m[i][j] += m[i][k] * other.m[k][j];

					}

				}

			}

			return result;

		}

		constexpr Matrix4x4& operator*=(const Matrix4x4& other) {
			
			(*this) = (*this) * other;

			return (*this);

		}


		constexpr Matrix4x4& operator*=(const float scalar) {

			for (size_t i = 0; i < 4; ++i) {

				for (size_t j = 0; j < 4; ++j) {

					m[i][j] *= scalar;

				}

			}

			return (*this);

		}

		constexpr Matrix4x4 operator*(const float scalar) {

			Matrix4x4 result = (*this);
			result *= scalar;

			return result;

		}

		Matrix4x4 Inversed() const;

		static Matrix4x4 InverseRT(const Matrix4x4& rotateMatrix, const Matrix4x4& translateMatrix);

		constexpr Matrix4x4 Transposed() const {

			Matrix4x4 result = {};

			for (size_t i = 0; i < 4; i++) {

				for (size_t j = 0; j < 4; j++) {

					result.m[i][j] = m[j][i];

				}

			}

			return result;

		}

		static constexpr Matrix4x4 Identity() {

			return Matrix4x4(
				{
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				}
			);

		}


		static constexpr Matrix4x4 Scale(const Vector3& scale) {

			return Matrix4x4(
				{
					scale.x, 0.0f, 0.0f, 0.0f,
					0.0f, scale.y, 0.0f, 0.0f,
					0.0f, 0.0f, scale.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				}
			);

		}

		static constexpr Matrix4x4 Translate(const Vector3& translate) {

			return Matrix4x4(
				{
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					translate.x, translate.y, translate.z, 1.0f
				}
			);

		}

		constexpr Vector3 Transform(const Vector3& vector) const {

			Vector3 result{};

			result.x = vector.x * m[0][0] + vector.y * m[1][0] + vector.z * m[2][0] + m[3][0];
			result.y = vector.x * m[0][1] + vector.y * m[1][1] + vector.z * m[2][1] + m[3][1];
			result.z = vector.x * m[0][2] + vector.y * m[1][2] + vector.z * m[2][2] + m[3][2];
			float w = vector.x * m[0][3] + vector.y * m[1][3] + vector.z * m[2][3] + m[3][3];

			assert(w != 0.0f && "Error: vector could not transform");

			result /= w;

			return result;

		}

		static constexpr Vector3 ScreenTransform(const Vector3& vector, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

			return viewportMatrix.Transform(viewProjectionMatrix.Transform(vector));

		}

		static Matrix4x4 RotateX(const float& angle);

		static Matrix4x4 RotateY(const float& angle);

		static Matrix4x4 RotateZ(const float& angle);

		static Matrix4x4 Rotate(const Vector3& rotate) {

			return RotateX(rotate.x) * RotateY(rotate.y) * RotateZ(rotate.z);

		}

		static Matrix4x4 World(const Vector3& translation, const Vector3& scale = Vector3{ 1.0f, 1.0f, 1.0f }, const Vector3& rotation = Vector3{ 0.0f, 0.0f, 0.0f });
		static Matrix4x4 World(const Vector3& translate, const Quaternion& rotate, const Vector3& scale);

		static Matrix4x4 Affine(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

			World(translate, scale, rotate);

		}

		// 透視投影行列
		static Matrix4x4 PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

		// 正射影行列
		static Matrix4x4 Orthographic(float left, float top, float right, float bottom, float nearClip, float farClip);

		// ビューポート変換行列
		static Matrix4x4 Viewport(float left, float top, float width, float height, float minDepth, float maxDepth);

		// LookAt行列
		static Matrix4x4 LookAt(const Vector3& observer, const Vector3& target, const Vector3& above);

		Vector3 ToEuler() const;


	};

	inline constexpr Matrix4x4 operator*(const float scalar, const Matrix4x4& matrix) {

		Matrix4x4 result = matrix;

		for (size_t i = 0; i < 4; ++i) {

			for (size_t j = 0; j < 4; ++j) {

				result.m[i][j] *= scalar;

			}

		}

		return result;

	}

}