#include "Math/Matrix2D.h"
#include "Math/Vector2.h"
#include <Assert.h>
#include <cmath>

Matrix3x3 MakeScaleMatrix(Vector2 const& scale) {

	Matrix3x3 result = { 0.0f };

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;

	result.m[2][2] = 1.0f;

	return result;

}

Matrix3x3 MakeRotateMatrix(float const& radian) {

	Matrix3x3 result = { 0.0f };

	result.m[0][0] = cosf(radian);
	result.m[0][1] = sinf(radian);
	result.m[1][0] = -sinf(radian);
	result.m[1][1] = cosf(radian);

	result.m[2][2] = 1.0f;

	return result;

}

Matrix3x3 MakeTranslateMatrix(Vector2 const& translate) {

	Matrix3x3 result = { 0.0f };

	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;

	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;

	return result;

}

Matrix3x3 MultiplyMatrix(Matrix3x3 const& matrixA, Matrix3x3 const& matrixB) {

	Matrix3x3 result = { 0.0f };

	result.m[0][0] = matrixA.m[0][0] * matrixB.m[0][0] + matrixA.m[0][1] * matrixB.m[1][0] + matrixA.m[0][2] * matrixB.m[2][0];
	result.m[0][1] = matrixA.m[0][0] * matrixB.m[0][1] + matrixA.m[0][1] * matrixB.m[1][1] + matrixA.m[0][2] * matrixB.m[2][1];
	result.m[0][2] = matrixA.m[0][0] * matrixB.m[0][2] + matrixA.m[0][1] * matrixB.m[1][2] + matrixA.m[0][2] * matrixB.m[2][2];

	result.m[1][0] = matrixA.m[1][0] * matrixB.m[0][0] + matrixA.m[1][1] * matrixB.m[1][0] + matrixA.m[1][2] * matrixB.m[2][0];
	result.m[1][1] = matrixA.m[1][0] * matrixB.m[0][1] + matrixA.m[1][1] * matrixB.m[1][1] + matrixA.m[1][2] * matrixB.m[2][1];
	result.m[1][2] = matrixA.m[1][0] * matrixB.m[0][2] + matrixA.m[1][1] * matrixB.m[1][2] + matrixA.m[1][2] * matrixB.m[2][2];

	result.m[2][0] = matrixA.m[2][0] * matrixB.m[0][0] + matrixA.m[2][1] * matrixB.m[1][0] + matrixA.m[2][2] * matrixB.m[2][0];
	result.m[2][1] = matrixA.m[2][0] * matrixB.m[0][1] + matrixA.m[2][1] * matrixB.m[1][1] + matrixA.m[2][2] * matrixB.m[2][1];
	result.m[2][2] = matrixA.m[2][0] * matrixB.m[0][2] + matrixA.m[2][1] * matrixB.m[1][2] + matrixA.m[2][2] * matrixB.m[2][2];

	return result;

}

Matrix3x3 MakeWorldMatrix(Vector2 const& translate, Vector2 const& scale, float const& radian) {

	Matrix3x3 result = { 0.0f };

	result.m[0][0] = scale.x * cosf(radian);
	result.m[0][1] = scale.x * sinf(radian);
	result.m[1][0] = -scale.y * sinf(radian);
	result.m[1][1] = scale.y * cosf(radian);
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;

	result.m[2][2] = 1.0f;

	return result;

}

Matrix3x3 MakeInverseMatrix(const Matrix3x3& matrix) {

	Matrix3x3 result{};

	float determinant = matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2]
		+ matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0]
		+ matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1]
		- matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0]
		- matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2]
		- matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1];

	result.m[0][0] = (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]) / determinant;
	result.m[0][1] = -(matrix.m[0][1] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][1]) / determinant;
	result.m[0][2] = (matrix.m[0][1] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][1]) / determinant;

	result.m[1][0] = -(matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]) / determinant;
	result.m[1][1] = (matrix.m[0][0] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][0]) / determinant;
	result.m[1][2] = -(matrix.m[0][0] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][0]) / determinant;

	result.m[2][0] = (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]) / determinant;
	result.m[2][1] = -(matrix.m[0][0] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[2][0]) / determinant;
	result.m[2][2] = (matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0]) / determinant;

	return result;

}

Matrix3x3 MakeOrthoMatrix(const Vector2& viewLeftTopLocal, const Vector2& viewRightBottomLocal) {

	Matrix3x3 result = { 0.0f };

	result.m[0][0] = 2.0f / (viewRightBottomLocal.x - viewLeftTopLocal.x);

	result.m[1][1] = 2.0f / (viewLeftTopLocal.y - viewRightBottomLocal.y);

	result.m[2][0] = (viewLeftTopLocal.x + viewRightBottomLocal.x) / (viewLeftTopLocal.x - viewRightBottomLocal.x);

	result.m[2][1] = (viewLeftTopLocal.y + viewRightBottomLocal.y) / (viewRightBottomLocal.y - viewLeftTopLocal.y);

	result.m[2][2] = 1.0f;

	return result;

}

Matrix3x3 MakeViewportMatrix(const Vector2& viewLeftTopPosScreen, const Vector2& viewSizeScreen) {

	Matrix3x3 result = { 0.0f };

	result.m[0][0] = viewSizeScreen.x / 2.0f;

	result.m[1][1] = -viewSizeScreen.y / 2.0f;

	result.m[2][0] = viewLeftTopPosScreen.x + viewSizeScreen.x / 2.0f;

	result.m[2][1] = viewLeftTopPosScreen.y + viewSizeScreen.y / 2.0f;

	result.m[2][2] = 1.0f;

	return result;

}

Vector2 VectorTransform(Vector2 const& vector, Matrix3x3 const& matrix) {

	Vector2 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];

	float w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];

	assert(w != 0.0f);

	result.x /= w;
	result.y /= w;

	return result;

}