#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include <cassert>
#include <cmath>

namespace Atrum::Math {

	static constexpr float Determinant3x3(
		const float& m00, const float& m01, const float& m02,
		const float& m10, const float& m11, const float& m12,
		const float& m20, const float& m21, const float& m22
	) {

		return m00 * (m11 * m22 - m12 * m21)
			- m01 * (m10 * m22 - m12 * m20)
			+ m02 * (m10 * m21 - m11 * m20);

	}

	Matrix4x4 Matrix4x4::Inversed() const {

		Matrix4x4 res;

		// 各要素の余因子を計算しつつ、転置の位置に代入
		// res.m[列][行] = 余因子 (符号に注意)

		// --- 1行目の余因子 (結果の1列目へ) ---
		res.m[0][0] = Determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
		res.m[1][0] = -Determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
		res.m[2][0] = Determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
		res.m[3][0] = -Determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

		// 行列式の計算 (1行目の展開)
		float det = m[0][0] * res.m[0][0] + m[0][1] * res.m[1][0] + m[0][2] * res.m[2][0] + m[0][3] * res.m[3][0];

		if (std::abs(det) < 0.00001f) {
			 // 逆行列なし

			return Matrix4x4{};

		}

		float invDet = 1.0f / det;

		// --- 残りの余因子を計算 (転置位置に代入) ---

		// 2行目の余因子 (結果の2列目へ)
		res.m[0][1] = -Determinant3x3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
		res.m[1][1] = Determinant3x3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
		res.m[2][1] = -Determinant3x3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
		res.m[3][1] = Determinant3x3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

		// 3行目の余因子 (結果の3列目へ)
		res.m[0][2] = Determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
		res.m[1][2] = -Determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
		res.m[2][2] = Determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
		res.m[3][2] = -Determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);

		// 4行目の余因子 (結果の4列目へ)
		res.m[0][3] = -Determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);
		res.m[1][3] = Determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);
		res.m[2][3] = -Determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);
		res.m[3][3] = Determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

		// 最後にスカラー(invDet)倍
		res *= invDet;

		return res;

	}

	Matrix4x4 Matrix4x4::InverseRT(const Matrix4x4& rotateMatrix, const Matrix4x4& translateMatrix) {

		Matrix4x4 result = Identity();

		Matrix4x4 rotateMatrixT = rotateMatrix.Transposed();

		Vector3 translate = { translateMatrix.m[3][0],translateMatrix.m[3][1],translateMatrix.m[3][2] };

		Vector3 translateConvert = rotateMatrixT.Transform(-1.0f * translate);

		float translateByArray[] = { translateConvert.x, translateConvert.y, translateConvert.z };

		for (size_t i = 0; i < 3; ++i) {

			result.m[3][i] = translateByArray[i];

			for (size_t j = 0; j < 3; ++j) {

				result.m[i][j] = rotateMatrixT.m[i][j];

			}

		}

		return result;

	}

	Matrix4x4 Matrix4x4::RotateX(const float& angle) {

		Matrix4x4 result = { 0.0f };

		result.m[0][0] = 1.0f;
		result.m[3][3] = 1.0f;

		result.m[1][1] = cosf(angle);
		result.m[1][2] = sinf(angle);
		result.m[2][1] = -sinf(angle);
		result.m[2][2] = cosf(angle);

		return result;

	}

	Matrix4x4 Matrix4x4::RotateY(const float& angle) {

		Matrix4x4 result = { 0.0f };

		result.m[1][1] = 1.0f;
		result.m[3][3] = 1.0f;

		result.m[0][0] = cosf(angle);
		result.m[0][2] = sinf(angle);
		result.m[2][0] = -sinf(angle);
		result.m[2][2] = cosf(angle);

		return result;

	}

	Matrix4x4 Matrix4x4::RotateZ(const float& angle) {

		Matrix4x4 result = { 0.0f };

		result.m[2][2] = 1.0f;
		result.m[3][3] = 1.0f;

		result.m[0][0] = cosf(angle);
		result.m[0][1] = sinf(angle);
		result.m[1][0] = -sinf(angle);
		result.m[1][1] = cosf(angle);

		return result;

	}


	Matrix4x4 Matrix4x4::World(const Vector3& translation, const Vector3& scale, const Vector3& rotation) {
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

	Matrix4x4 Matrix4x4::World(const Vector3& translate, const Quaternion& rotate, const Vector3& scale) {

		return Scale(scale) * rotate.MakeRotateMatrixRh() * Translate(translate);

	}

	Matrix4x4 Matrix4x4::PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {

		float cotangent = 1.0f / (tan(fovY * 0.5f));

		Matrix4x4 result{};

		result.m[0][0] = cotangent / aspectRatio;

		result.m[1][1] = cotangent;

		result.m[2][2] = farClip / (farClip - nearClip);

		result.m[3][2] = -nearClip * farClip / (farClip - nearClip);

		result.m[2][3] = 1.0f;

		return result;

	}

	Matrix4x4 Matrix4x4::Orthographic(float left, float top, float right, float bottom, float nearClip, float farClip) {

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

	Matrix4x4 Matrix4x4::Viewport(float left, float top, float width, float height, float minDepth, float maxDepth) {

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

	Matrix4x4 Matrix4x4::LhLookAt(const Vector3& target, const Vector3& eye, const Vector3& up) {
		return LhLookAt(target - eye, up);
	}

	Matrix4x4 Matrix4x4::LhLookAt(const Vector3& forward, const Vector3& up) {
		Vector3 f = forward.Normalized();
		Vector3 s = up.CrossLh(f).Normalized();
		Vector3 u = f.CrossLh(s);

		Matrix4x4 m = Identity();
		m.m[0][0] = s.x; m.m[0][1] = s.y; m.m[0][2] = s.z;
		m.m[1][0] = u.x; m.m[1][1] = u.y; m.m[1][2] = u.z;
		m.m[2][0] = f.x; m.m[2][1] = f.y; m.m[2][2] = f.z;
		return m;
	}

	Matrix4x4 Matrix4x4::RhLookAt(const Vector3& target, const Vector3& eye, const Vector3& up) {

		return RhLookAt(target - eye, up);

	}

	Matrix4x4 Matrix4x4::RhLookAt(const Vector3& forward, const Vector3& up) {

		Vector3 f = forward.Normalized();
		Vector3 s = Physics::Cross(up, f).Normalized();
		Vector3 u = Physics::Cross(f, s);

		Matrix4x4 m = Identity();
		m.m[0][0] = s.x; m.m[0][1] = s.y; m.m[0][2] = s.z;
		m.m[1][0] = u.x; m.m[1][1] = u.y; m.m[1][2] = u.z;
		m.m[2][0] = f.x; m.m[2][1] = f.y; m.m[2][2] = f.z;


		return m;

	}

	Vector3 Matrix4x4::ToEuler() const {
		Vector3 rotation{};

		// Pitch (X)
		rotation.x = asin(-m[1][2]);

		// ジンバルロックの判定 (cos(pitch) が 0 に近い場合)
		if (cos(rotation.x) > 0.0001f) {
			// Yaw (Y)
			rotation.y = atan2(m[0][2], m[2][2]);
			// Roll (Z)
			rotation.z = atan2(m[1][0], m[1][1]);
		} else {
			// ジンバルロック時の処理（一例）
			rotation.y = atan2(-m[2][0], m[0][0]);
			rotation.z = 0.0f;
		}

		return rotation;
	}

}