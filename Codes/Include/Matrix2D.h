#pragma once
#include "Vector2.h"

struct Matrix3x3 {
	float m[3][3];
};

Matrix3x3 MakeScaleMatrix(Vector2 const& scale);

Matrix3x3 MakeRotateMatrix(float const& radian);

Matrix3x3 MakeTranslateMatrix(Vector2 const& translate);

Matrix3x3 MultiplyMatrix(Matrix3x3 const& matrixA, Matrix3x3 const& matrixB);

Matrix3x3 MakeWorldMatrix(Vector2 const& translate, Vector2 const& scale = { 1.0f, 1.0f }, float const& radian = 0.0f);

Matrix3x3 MakeInverseMatrix(const Matrix3x3& matrix);

Matrix3x3 MakeOrthoMatrix(const Vector2& viewLeftTopLocal, const Vector2& viewRightBottomLocal);

Matrix3x3 MakeViewportMatrix(const Vector2& viewLeftTopPosScreen, const Vector2& viewSizeScreen);

Vector2 VectorTransform(Vector2 const& vector, Matrix3x3 const& matrix);