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

	constexpr Quaternion operator*(const Quaternion& other) {

		return Quaternion{
			w * other.w - x * other.x - y * other.y - z * other.z,
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - y * other.w + z * other.x - x * other.z,
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

		const auto& rMat = rotateMatrix.m;

		float trace = rMat[0][0] + rMat[1][1] + rMat[2][2];

		Quaternion quaternion{};

		if (trace > 0.0f) {

			float sin = 0.5f / std::sqrt(trace + 1.0f);
			quaternion.w = 0.25f / sin;
			quaternion.x = (rMat[2][1] - rMat[1][2]) * sin;
			quaternion.y = (rMat[0][2] - rMat[2][0]) * sin;
			quaternion.z = (rMat[1][0] - rMat[0][1]) * sin;
		
		} else {
			
			if (rMat[0][0] > rMat[1][1] && rMat[0][0] > rMat[2][2]) {
			
				float sin = 2.0f * std::sqrt(1.0f + rMat[0][0] - rMat[1][1] - rMat[2][2]);
				quaternion.w = (rMat[2][1] - rMat[1][2]) / sin;
				quaternion.x = 0.25f * sin;
				quaternion.y = (rMat[0][1] + rMat[1][0]) / sin;
				quaternion.z = (rMat[0][2] + rMat[2][0]) / sin;
			
			} else if (rMat[1][1] > rMat[2][2]) {
		
				float sin = 2.0f * std::sqrt(1.0f + rMat[1][1] - rMat[0][0] - rMat[2][2]);
				quaternion.w = (rMat[0][2] - rMat[2][0]) / sin;
				quaternion.x = (rMat[0][1] + rMat[1][0]) / sin;
				quaternion.y = 0.25f * sin;
				quaternion.z = (rMat[1][2] + rMat[2][1]) / sin;
			
			} else {
			
				float sin = 2.0f * std::sqrt(1.0f + rMat[2][2] - rMat[0][0] - rMat[1][1]);
				quaternion.w = (rMat[1][0] - rMat[0][1]) / sin;
				quaternion.x = (rMat[0][2] + rMat[2][0]) / sin;
				quaternion.y = (rMat[1][2] + rMat[2][1]) / sin;
				quaternion.z = 0.25f * sin;
			
			}

		}

		return quaternion;

	}

	static Quaternion FromEulerRotateVector(const Vector3& rotate) {

		return FromEulerRotateMatrix(MakeRotateMatrix(rotate));

	}

	Vector3 rotate_vector(const Vector3& vector) {

		Quaternion vq{0, vector.x, vector.y, vector.z};
		Quaternion inv = conjugated();
		Quaternion result = (*this) * vq * inv;
		return Vector3{ result.x, result.y, result.z };

	}

	void add_rotation(const Quaternion& delta) {

		*this *= delta;

		normalize();

	}

};