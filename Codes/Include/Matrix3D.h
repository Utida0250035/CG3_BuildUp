#pragma once

#include "Vector3.h"
#include <cassert>

struct Matrix4x4 {
	float m[4][4]{};
};

inline constexpr Matrix4x4 operator+(const Matrix4x4& me, const Matrix4x4& other) {

	return Matrix4x4(
		{
			me.m[0][0] + other.m[0][0], me.m[0][1] + other.m[0][1], me.m[0][2] + other.m[0][2], me.m[0][3] + other.m[0][3],
			me.m[1][0] + other.m[1][0], me.m[1][1] + other.m[1][1], me.m[1][2] + other.m[1][2], me.m[1][3] + other.m[1][3],
			me.m[2][0] + other.m[2][0], me.m[2][1] + other.m[2][1], me.m[2][2] + other.m[2][2], me.m[2][3] + other.m[2][3],
			me.m[3][0] + other.m[3][0], me.m[3][1] + other.m[3][1], me.m[3][2] + other.m[3][2], me.m[3][3] + other.m[3][3]
		}
	);

}

inline constexpr void operator+=(Matrix4x4& me, const Matrix4x4& other) {

	me = Matrix4x4(
		{
			me.m[0][0] + other.m[0][0], me.m[0][1] + other.m[0][1], me.m[0][2] + other.m[0][2], me.m[0][3] + other.m[0][3],
			me.m[1][0] + other.m[1][0], me.m[1][1] + other.m[1][1], me.m[1][2] + other.m[1][2], me.m[1][3] + other.m[1][3],
			me.m[2][0] + other.m[2][0], me.m[2][1] + other.m[2][1], me.m[2][2] + other.m[2][2], me.m[2][3] + other.m[2][3],
			me.m[3][0] + other.m[3][0], me.m[3][1] + other.m[3][1], me.m[3][2] + other.m[3][2], me.m[3][3] + other.m[3][3]
		}
	);

}

inline constexpr Matrix4x4 operator-(const Matrix4x4& me, const Matrix4x4& other) {

	return Matrix4x4(
		{
			me.m[0][0] - other.m[0][0], me.m[0][1] - other.m[0][1], me.m[0][2] - other.m[0][2], me.m[0][3] - other.m[0][3],
			me.m[1][0] - other.m[1][0], me.m[1][1] - other.m[1][1], me.m[1][2] - other.m[1][2], me.m[1][3] - other.m[1][3],
			me.m[2][0] - other.m[2][0], me.m[2][1] - other.m[2][1], me.m[2][2] - other.m[2][2], me.m[2][3] - other.m[2][3],
			me.m[3][0] - other.m[3][0], me.m[3][1] - other.m[3][1], me.m[3][2] - other.m[3][2], me.m[3][3] - other.m[3][3]
		}
	);

}

inline constexpr void operator-=(Matrix4x4& me, const Matrix4x4& other) {

	me = Matrix4x4(
		{
			me.m[0][0] - other.m[0][0], me.m[0][1] - other.m[0][1], me.m[0][2] - other.m[0][2], me.m[0][3] - other.m[0][3],
			me.m[1][0] - other.m[1][0], me.m[1][1] - other.m[1][1], me.m[1][2] - other.m[1][2], me.m[1][3] - other.m[1][3],
			me.m[2][0] - other.m[2][0], me.m[2][1] - other.m[2][1], me.m[2][2] - other.m[2][2], me.m[2][3] - other.m[2][3],
			me.m[3][0] - other.m[3][0], me.m[3][1] - other.m[3][1], me.m[3][2] - other.m[3][2], me.m[3][3] - other.m[3][3]
		}
	);

}

inline constexpr Matrix4x4 operator*(const Matrix4x4& me, const Matrix4x4& other) {

	Matrix4x4 result = {};

	for (size_t i = 0; i < 4; i++) {

		for (size_t j = 0; j < 4; j++) {

			for (size_t k = 0; k < 4; k++) {

				result.m[i][j] += me.m[i][k] * other.m[k][j];

			}

		}

	}

	return result;

}

inline constexpr void operator*=(Matrix4x4& me, const Matrix4x4& other) {

	Matrix4x4 result = {};

	for (size_t i = 0; i < 4; i++) {

		for (size_t j = 0; j < 4; j++) {

			for (size_t k = 0; k < 4; k++) {

				result.m[i][j] += me.m[i][k] * other.m[k][j];

			}

		}

	}

	me = result;

}

inline constexpr Matrix4x4 operator*(const float scalar, const Matrix4x4& matrix) {

	Matrix4x4 result{};

	for (size_t i = 0; i < 4; ++i) {

		for (size_t j = 0; j < 4; ++j) {

			result.m[i][j] = matrix.m[i][j] * scalar;

		}

	}

	return result;

}

inline constexpr Matrix4x4 operator*(const Matrix4x4& matrix, const float scalar) {

	return scalar * matrix;

}

inline constexpr void operator*=(Matrix4x4& matrix, const float scalar) {

	matrix = scalar * matrix;

}

inline constexpr Matrix4x4 Add(const Matrix4x4& me, const Matrix4x4& other) {

	return me + other;

}

inline constexpr Matrix4x4 Subtract(const Matrix4x4& me, const Matrix4x4& other) {

	return me - other;

}

inline constexpr Matrix4x4 Multiply(const Matrix4x4& me, const Matrix4x4& other) {

	return me * other;

}

/// <summary>
/// 3x3の行列式を求める補助関数
/// </summary>
/// <param name="m00"></param>
/// <param name="m01"></param>
/// <param name="m02"></param>
/// <param name="m10"></param>
/// <param name="m11"></param>
/// <param name="m12"></param>
/// <param name="m20"></param>
/// <param name="m21"></param>
/// <param name="m22"></param>
/// <returns></returns>
inline constexpr float Determinant3x3(
	const float& m00, const float& m01, const float& m02,
	const float& m10, const float& m11, const float& m12,
	const float& m20, const float& m21, const float& m22
) {

	return m00 * (m11 * m22 - m12 * m21)
		- m01 * (m10 * m22 - m12 * m20)
		+ m02 * (m10 * m21 - m11 * m20);

}

Matrix4x4 MatrixInverse(const Matrix4x4& matrix);

Matrix4x4 RTMatrixInverse(const Matrix4x4& rotateMatrix, const Matrix4x4& translateMatrix);

inline constexpr Matrix4x4 MatrixTranspose(const Matrix4x4 matrix) {

	Matrix4x4 result = {};

	for (size_t i = 0; i < 4; i++) {

		for (size_t j = 0; j < 4; j++) {

			result.m[i][j] = matrix.m[j][i];

		}

	}

	return result;

}

inline constexpr Matrix4x4 MakeIdentity4x4() {

	return Matrix4x4(
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		}
	);

}


inline constexpr Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	return Matrix4x4(
		{
			scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, scale.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		}
	);

}

inline constexpr Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

	return Matrix4x4(
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			translate.x, translate.y, translate.z, 1.0f
		}
	);

}

inline constexpr Vector3 VectorTransform(const Vector3& vector, const Matrix4x4& matrix) {

	Vector3 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

	assert(w != 0.0f && "Error: vector could not transform");

	result /= w;

	return result;

}

inline constexpr Vector3 ScreenTransform(const Vector3& vector, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	return VectorTransform(VectorTransform(vector, viewProjectionMatrix), viewportMatrix);

}

Matrix4x4 MakeXRotateMatrix(const float& angle);

Matrix4x4 MakeYRotateMatrix(const float& angle);

Matrix4x4 MakeZRotateMatrix(const float& angle);

inline constexpr Matrix4x4 MakeRotateMatrix(const Vector3& rotate) {

	return MakeXRotateMatrix(rotate.x) * MakeYRotateMatrix(rotate.y) * MakeZRotateMatrix(rotate.z);

}

Matrix4x4 MakeWorldMatrix(const Vector3& translation, const Vector3& scale = Vector3{ 1.0f, 1.0f, 1.0f }, const Vector3& rotation = Vector3{0.0f, 0.0f, 0.0f});

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

static constexpr int kMatrixPrintRowHeight = 20;
static constexpr int kMatrixPrintColumnWidth = 60;

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// LookAt行列
Matrix4x4 MakeLookAtMatrix(const Vector3& observer, const Vector3& target, const Vector3& above);