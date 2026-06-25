#pragma once

#define NOMINMAX
#include "Vector3.h"
#include "Matrix3D.h"
#include <algorithm>
#include <cassert>
#include <cmath>

struct Quaternion {
	float w = 1.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	constexpr float magnitude_square() const { return  w * w + x * x + y * y + z * z; }
	float magnitude() const { return std::sqrt(magnitude_square()); }

	void normalize() {
		float magnitude = this->magnitude();

		if (magnitude > 0.0f) {

			w /= magnitude;
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;

			return;

		}

		assert(false);

		*this = {};

	}

	Quaternion normalized() const {

		Quaternion q = *this;

		q.normalize();

		return q;

	}

	constexpr Quaternion operator*(const Quaternion& other) const
	{
		return Quaternion{
			w * other.w - x * other.x - y * other.y - z * other.z,

			w * other.x + x * other.w + y * other.z - z * other.y,

			w * other.y + y * other.w + z * other.x - x * other.z,

			w * other.z + z * other.w + x * other.y - y * other.x
		};
	}

	constexpr void operator*=(const Quaternion& other) {

		*this = (*this) * other;

	}

	//float get_theta() {
	//	assert(std::abs(magnitude_square() - 1.0f) < 0.001f);

	//	Quaternion nq = normalized();

	//	float clamped_w = std::max(-1.0f, std::min(1.0f, nq.w));
	//	return 2.0f * std::acos(clamped_w);
	//}

	Quaternion conjugated() const {
		return Quaternion(w, -x, -y, -z);
	}

	Matrix4x4 create_rotate_matrix() const {

		//assert(std::abs(magnitude_square() - 1.0f) < 0.001f);

		Quaternion nq = normalized();

		float x2 = nq.x * nq.x;
		float y2 = nq.y * nq.y;
		float z2 = nq.z * nq.z;
		float xy = nq.x * nq.y;
		float xz = nq.x * nq.z;
		float yz = nq.y * nq.z;
		float wx = nq.w * nq.x;
		float wy = nq.w * nq.y;
		float wz = nq.w * nq.z;

		Matrix4x4 rotateMatrix = MakeIdentity4x4();

		// 行優先(Row-Major)で回転成分のみを上書き
		rotateMatrix.m[0][0] = 1.0f - 2.0f * (y2 + z2);
		rotateMatrix.m[0][1] = 2.0f * (xy - wz);
		rotateMatrix.m[0][2] = 2.0f * (xz + wy);

		rotateMatrix.m[1][0] = 2.0f * (xy + wz);
		rotateMatrix.m[1][1] = 1.0f - 2.0f * (x2 + z2);
		rotateMatrix.m[1][2] = 2.0f * (yz - wx);

		rotateMatrix.m[2][0] = 2.0f * (xz - wy);
		rotateMatrix.m[2][1] = 2.0f * (yz + wx);
		rotateMatrix.m[2][2] = 1.0f - 2.0f * (x2 + y2);

		return rotateMatrix;

	}

	static Quaternion FromAxisAngle(const Vector3 unitVector, const float angle) {

		assert(std::abs(VectorLength(unitVector) - 1.0f) < 0.001f);

		float halfAngle = angle * 0.5f;
		float sin = std::sin(halfAngle);
		return Quaternion{std::cos(halfAngle), unitVector.x * sin, unitVector.y * sin, unitVector.z * sin};
	}

	static Quaternion FromEulerRotateMatrix(const Matrix4x4& rotateMatrix) {

		const auto& m = rotateMatrix.m;
		Quaternion q;
		float trace = m[0][0] + m[1][1] + m[2][2];

		if (trace > 0.0f) {
			float s = 0.5f / std::sqrt(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (m[2][1] - m[1][2]) * s;
			q.y = (m[0][2] - m[2][0]) * s;
			q.z = (m[1][0] - m[0][1]) * s;
		} else {
			if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) {
				float s = 2.0f * std::sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]);
				q.w = (m[2][1] - m[1][2]) / s;
				q.x = 0.25f * s;
				q.y = (m[0][1] + m[1][0]) / s;
				q.z = (m[0][2] + m[2][0]) / s;
			} else if (m[1][1] > m[2][2]) {
				float s = 2.0f * std::sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]);
				q.w = (m[0][2] - m[2][0]) / s;
				q.x = (m[0][1] + m[1][0]) / s;
				q.y = 0.25f * s;
				q.z = (m[1][2] + m[2][1]) / s;
			} else {
				float s = 2.0f * std::sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]);
				q.w = (m[1][0] - m[0][1]) / s;
				q.x = (m[0][2] + m[2][0]) / s;
				q.y = (m[1][2] + m[2][1]) / s;
				q.z = 0.25f * s;
			}
		}

		return q.normalized(); // 最後に必ず正規化する

	}

	static Quaternion FromEulerRotateVector(const Vector3& rotate) {

		return FromEulerRotateMatrix(MakeRotateMatrix(rotate));

	}

	Vector3 rotate_vector(const Vector3& vector) {
		
		Quaternion p{ 0, vector.x, vector.y, vector.z };

		Quaternion r =
			(*this) * p * this->conjugated();

		return { r.x, r.y, r.z };

	}

	void add_rotation(const Quaternion& delta) {

		(*this) *= delta;

		this->normalize();

	}

	static Quaternion FromTwoDirection(Vector3 from, Vector3 to) {

		from = VectorNormalize(from);
		to = VectorNormalize(to);

		float dot = VectorDot(from, to);

		if (dot < -0.99999f) {

			Vector3 axis = VectorCross({1.0f, 0.0f, 0.0f}, from);

			if (VectorLength(axis) < 0.001f) {

				axis = VectorCross({0.0f, 1.0f, 0.0f}, from);

			}

			return Quaternion{0.0f, axis.x, axis.y, axis.z};

		}

		Vector3 axis = VectorCross(from, to);
		Quaternion q = { dot + 1.0f, axis.x, axis.y, axis.z };

		return q.normalized();

	}

};

inline Quaternion LookAtRotation(Vector3 pos, Vector3 target, Vector3 up) {
	// 1. Forwardベクトルを計算 (ターゲット - 位置)
	Vector3 f = VectorNormalize(target - pos);

	// 2. Rightベクトルを計算
	Vector3 r = VectorNormalize(VectorCross(f, up));

	// 3. Upベクトルを再計算
	Vector3 u = VectorCross(r, f);

	// 4. 回転行列を作成
	// ここで重要なのは「列」に並べること（回転行列 R の各列が基底ベクトルになる）
	// 行列クラスの仕様が [row][col] なら以下のように並べる
	Matrix4x4 m;
	m.m[0][0] = r.x; m.m[1][0] = r.y; m.m[2][0] = r.z;
	m.m[0][1] = u.x; m.m[1][1] = u.y; m.m[2][1] = u.z;
	m.m[0][2] = -f.x; m.m[1][2] = -f.y; m.m[2][2] = -f.z;
	m.m[0][3] = 0.0f; m.m[1][3] = 0.0f; m.m[2][3] = 0.0f; m.m[3][3] = 1.0f;

	// 5. 行列をクォータニオンに変換
	return Quaternion::FromEulerRotateMatrix(m).normalized().conjugated();

}