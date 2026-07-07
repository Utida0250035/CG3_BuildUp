#include "Matrix3D.h"
#include <cassert>
#include <cmath>

Matrix4x4 MatrixInverse(const Matrix4x4& matrix) {

	Matrix4x4 res;

	// 各要素の余因子を計算しつつ、転置の位置に代入
	// res.m[列][行] = 余因子 (符号に注意)

	// --- 1行目の余因子 (結果の1列目へ) ---
	res.m[0][0] = Determinant3x3(matrix.m[1][1], matrix.m[1][2], matrix.m[1][3], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
	res.m[1][0] = -Determinant3x3(matrix.m[1][0], matrix.m[1][2], matrix.m[1][3], matrix.m[2][0], matrix.m[2][2], matrix.m[2][3], matrix.m[3][0], matrix.m[3][2], matrix.m[3][3]);
	res.m[2][0] = Determinant3x3(matrix.m[1][0], matrix.m[1][1], matrix.m[1][3], matrix.m[2][0], matrix.m[2][1], matrix.m[2][3], matrix.m[3][0], matrix.m[3][1], matrix.m[3][3]);
	res.m[3][0] = -Determinant3x3(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);

	// 行列式の計算 (1行目の展開)
	float det = matrix.m[0][0] * res.m[0][0] + matrix.m[0][1] * res.m[1][0] + matrix.m[0][2] * res.m[2][0] + matrix.m[0][3] * res.m[3][0];

	if (std::abs(det) < 0.00001f) {
		 // 逆行列なし

		return Matrix4x4{};

	}

	float invDet = 1.0f / det;

	// --- 残りの余因子を計算 (転置位置に代入) ---

	// 2行目の余因子 (結果の2列目へ)
	res.m[0][1] = -Determinant3x3(matrix.m[0][1], matrix.m[0][2], matrix.m[0][3], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
	res.m[1][1] = Determinant3x3(matrix.m[0][0], matrix.m[0][2], matrix.m[0][3], matrix.m[2][0], matrix.m[2][2], matrix.m[2][3], matrix.m[3][0], matrix.m[3][2], matrix.m[3][3]);
	res.m[2][1] = -Determinant3x3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][3], matrix.m[2][0], matrix.m[2][1], matrix.m[2][3], matrix.m[3][0], matrix.m[3][1], matrix.m[3][3]);
	res.m[3][1] = Determinant3x3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);

	// 3行目の余因子 (結果の3列目へ)
	res.m[0][2] = Determinant3x3(matrix.m[0][1], matrix.m[0][2], matrix.m[0][3], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
	res.m[1][2] = -Determinant3x3(matrix.m[0][0], matrix.m[0][2], matrix.m[0][3], matrix.m[1][0], matrix.m[1][2], matrix.m[1][3], matrix.m[3][0], matrix.m[3][2], matrix.m[3][3]);
	res.m[2][2] = Determinant3x3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][3], matrix.m[1][0], matrix.m[1][1], matrix.m[1][3], matrix.m[3][0], matrix.m[3][1], matrix.m[3][3]);
	res.m[3][2] = -Determinant3x3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);

	// 4行目の余因子 (結果の4列目へ)
	res.m[0][3] = -Determinant3x3(matrix.m[0][1], matrix.m[0][2], matrix.m[0][3], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3]);
	res.m[1][3] = Determinant3x3(matrix.m[0][0], matrix.m[0][2], matrix.m[0][3], matrix.m[1][0], matrix.m[1][2], matrix.m[1][3], matrix.m[2][0], matrix.m[2][2], matrix.m[2][3]);
	res.m[2][3] = -Determinant3x3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][3], matrix.m[1][0], matrix.m[1][1], matrix.m[1][3], matrix.m[2][0], matrix.m[2][1], matrix.m[2][3]);
	res.m[3][3] = Determinant3x3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);

	// 最後にスカラー(invDet)倍
	res *= invDet;

	return res;

}

Matrix4x4 RTMatrixInverse(const Matrix4x4& rotateMatrix, const Matrix4x4& translateMatrix) {

	Matrix4x4 result = MakeIdentity4x4();

	Matrix4x4 rotateMatrixT = MatrixTranspose(rotateMatrix);

	Vector3 translate = { translateMatrix.m[3][0],translateMatrix.m[3][1],translateMatrix.m[3][2] };

	Vector3 translateConvert = VectorTransform(-1.0f * translate, rotateMatrixT);

	float translateByArray[] = { translateConvert.x, translateConvert.y, translateConvert.z };

	for (size_t i = 0; i < 3; ++i) {

		result.m[3][i] = translateByArray[i];

		for (size_t j = 0; j < 3; ++j) {

			result.m[i][j] = rotateMatrixT.m[i][j];

		}

	}

	return result;

}

Matrix4x4 MakeXRotateMatrix(const float& angle) {

	Matrix4x4 result = { 0.0f };

	result.m[0][0] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[1][1] = cosf(angle);
	result.m[1][2] = sinf(angle);
	result.m[2][1] = -sinf(angle);
	result.m[2][2] = cosf(angle);

	return result;

}

Matrix4x4 MakeYRotateMatrix(const float& angle) {

	Matrix4x4 result = { 0.0f };

	result.m[1][1] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[0][0] = cosf(angle);
	result.m[0][2] = sinf(angle);
	result.m[2][0] = -sinf(angle);
	result.m[2][2] = cosf(angle);

	return result;

}

Matrix4x4 MakeZRotateMatrix(const float& angle) {

	Matrix4x4 result = { 0.0f };

	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[0][0] = cosf(angle);
	result.m[0][1] = sinf(angle);
	result.m[1][0] = -sinf(angle);
	result.m[1][1] = cosf(angle);

	return result;

}


Matrix4x4 MakeWorldMatrix(const Vector3& translation, const Vector3& scale, const Vector3& rotation) {
	float cx = cosf(rotation.x); float sx = sinf(rotation.x);
	float cy = cosf(rotation.y); float sy = sinf(rotation.y);
	float cz = cosf(rotation.z); float sz = sinf(rotation.z);

	return Matrix4x4{

		// 1,1([0][0])
		scale.x * (cy * cz),
		// 1,2([0][1])
		scale.x * (cy * sz),
		// 1,3([0][2])
		scale.x * (-sy),
		// 1,4([0][3])
		0.0f,

		// 2,1([1][0])
		scale.y * (sx * sy * cz - cx * sz),
		// 2,2([1][1])
		scale.y * (sx * sy * sz + cx * cz),
		// 2,3([1][2])
		scale.y * (sx * cy),
		// 2,4([1][3])
		0.0f,

		// 3,1([2][0])
		scale.z * (cx * sy * cz + sx * sz),
		// 3,2([2][1])
		scale.z * (cx * sy * sz - sx * cz),
		// 3,3([2][2])
		scale.z * (cx * cy),
		// 3,4([2][3])
		0.0f,

		// 4,1([3][0])
		translation.x,
		// 4,2([3][1])
		translation.y,
		// 4,3([3][2])
		translation.z,
		// 4,4([3][3])
		1.0f

	};
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	Matrix4x4 rotateMatrix = Multiply(Multiply(MakeXRotateMatrix(rotate.x), MakeYRotateMatrix(rotate.y)), MakeZRotateMatrix(rotate.z));

	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	return Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {

	float cotangent = 1.0f / (tan(fovY * 0.5f));

	Matrix4x4 result{};

	result.m[0][0] = cotangent / aspectRatio;

	result.m[1][1] = cotangent;

	result.m[2][2] = farClip / (farClip - nearClip);

	result.m[3][2] = -nearClip * farClip / (farClip - nearClip);

	result.m[2][3] = 1.0f;

	return result;

}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {

	Matrix4x4 result{};

	result.m[0][0] = 2.0f / (right - left);

	result.m[1][1] = 2.0f / (top - bottom);

	result.m[2][2] = 1.0f / (farClip - nearClip);

	result.m[3][0] = (left + right) / (left - right);

	result.m[3][1] = (top + bottom) / (bottom - top);

	result.m[3][2] = nearClip / (nearClip - farClip);

	result.m[3][3] = 1.0f;

	return result;

}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {

	Matrix4x4 result{};

	result.m[0][0] = width * 0.5f;

	result.m[1][1] = -height * 0.5f;

	result.m[2][2] = maxDepth - minDepth;

	result.m[3][0] = left + width * 0.5f;

	result.m[3][1] = top + height * 0.5f;

	result.m[3][2] = minDepth;

	result.m[3][3] = 1.0f;

	return result;

}

Matrix4x4 MakeLookAtMatrix(const Vector3& observerPos, const Vector3& targetPos, const Vector3& above) {

	Vector3 f = VectorNormalize(targetPos - observerPos);

	Vector3 r = VectorNormalize(VectorCross(f, above));

	Vector3 u = VectorNormalize(VectorCross(r, f));

	Matrix4x4 m = MakeIdentity4x4();

	// 回転成分 (左上 3x3)
	m.m[0][0] = r.x; m.m[0][1] = r.y; m.m[0][2] = r.z;
	m.m[1][0] = u.x; m.m[1][1] = u.y; m.m[1][2] = u.z;
	m.m[2][0] = -f.x; m.m[2][1] = -f.y; m.m[2][2] = -f.z;


	m.m[3][0] = -VectorDot(r, observerPos);
	m.m[3][1] = -VectorDot(u, observerPos);
	m.m[3][2] = VectorDot(f, observerPos);

	return m;

}

Vector3 MatrixToEuler(const Matrix4x4& m) {
	Vector3 rotation{};

	// Pitch (X)
	rotation.x = asin(-m.m[1][2]);

	// ジンバルロックの判定 (cos(pitch) が 0 に近い場合)
	if (cos(rotation.x) > 0.0001f) {
		// Yaw (Y)
		rotation.y = atan2(m.m[0][2], m.m[2][2]);
		// Roll (Z)
		rotation.z = atan2(m.m[1][0], m.m[1][1]);
	} else {
		// ジンバルロック時の処理（一例）
		rotation.y = atan2(-m.m[2][0], m.m[0][0]);
		rotation.z = 0.0f;
	}

	return rotation;
}