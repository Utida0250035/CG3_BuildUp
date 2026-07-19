#pragma once
#include "Vector2.h"

namespace Atrum::Math {

	struct Matrix3x3 {
		float m[3][3];

		static Matrix3x3 Scale(Vector2 const& scale);

		static Matrix3x3 Rotate(float const& radian);

		static Matrix3x3 Translate(Vector2 const& translate);

		static Matrix3x3 Multiply(Matrix3x3 const& matrixA, Matrix3x3 const& matrixB);

		static Matrix3x3 World(const Vector2& translate, Vector2 const& scale = { 1.0f, 1.0f }, float const& radian = 0.0f);

		static Matrix3x3 Ortho(const Vector2& viewLeftTopLocal, const Vector2& viewRightBottomLocal);

		static Matrix3x3 Viewport(const Vector2& viewLeftTopPosScreen, const Vector2& viewSizeScreen);

		Matrix3x3 Inversed() const;

		Vector2 VectorTransform(const Vector2& vector) const;

	};

}