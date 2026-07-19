#pragma once
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Atrum::Math {

	namespace std = ::std;

	struct Quaternion {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;

		[[nodiscard]] static constexpr Quaternion Identity() {

			return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
		}

		[[nodiscard]] constexpr float MagnitudeSquare() const { return  w * w + x * x + y * y + z * z; }
		[[nodiscard]] float Magnitude() const { return std::sqrt(MagnitudeSquare()); }

		void Normalize() {
			float magnitude = this->Magnitude();

			if (magnitude > 0.0f) {

				x /= magnitude;
				y /= magnitude;
				z /= magnitude;
				w /= magnitude;

				return;

			}

			assert(false);

			*this = {};

		}

		[[nodiscard]] Quaternion Normalized() const {

			Quaternion q = *this;

			q.Normalize();

			return q;

		}

		[[nodiscard]] constexpr Quaternion operator*(const Quaternion& other) const
		{
			return Quaternion{

				w* other.x + x * other.w + y * other.z - z * other.y,

					w* other.y + y * other.w + z * other.x - x * other.z,

					w* other.z + z * other.w + x * other.y - y * other.x,

					w* other.w - x * other.x - y * other.y - z * other.z
			};
		}

		[[nodiscard]] constexpr void operator*=(const Quaternion& other) {

			*this = (*this) * other;

		}

		[[nodiscard]] float GetTheta() {
			assert(std::abs(MagnitudeSquare() - 1.0f) < 0.001f);

			Quaternion nq = Normalized();

			float clamped_w = std::max(-1.0f, std::min(1.0f, nq.w));
			return 2.0f * std::acos(clamped_w);
		}

		[[nodiscard]] Quaternion Conjugated() const {
			return Quaternion(-x, -y, -z, w);
		}

		[[nodiscard]] Matrix4x4 CreateRotateMatrix() const {

			assert(std::abs(MagnitudeSquare() - 1.0f) < 0.001f);

			Quaternion nq = Normalized();

			float x2 = nq.x * nq.x;
			float y2 = nq.y * nq.y;
			float z2 = nq.z * nq.z;
			float xy = nq.x * nq.y;
			float xz = nq.x * nq.z;
			float yz = nq.y * nq.z;
			float wx = nq.w * nq.x;
			float wy = nq.w * nq.y;
			float wz = nq.w * nq.z;

			Matrix4x4 rotateMatrix = Matrix4x4::Identity();

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

			assert(std::abs(unitVector.LengthSquare() - 1.0f) < 0.001f);

			float halfAngle = angle * 0.5f;
			float sin = std::sin(halfAngle);
			return Quaternion{ unitVector.x * sin, unitVector.y * sin, -unitVector.z * sin, std::cos(halfAngle) };

		}

		static Quaternion FromRotateMatrix(const Matrix4x4& rotateMatrix) {

			const auto& m = rotateMatrix.m;
			Quaternion q;
			float trace = m[0][0] + m[1][1] + m[2][2];

			if (trace > 0.0f) {
				float s = 0.5f / std::sqrt(trace + 1.0f);
				q.x = (m[2][1] - m[1][2]) * s;
				q.y = (m[0][2] - m[2][0]) * s;
				q.z = (m[1][0] - m[0][1]) * s;
				q.w = 0.25f / s;
			} else {
				if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) {
					float s = 2.0f * std::sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]);
					q.x = 0.25f * s;
					q.y = (m[0][1] + m[1][0]) / s;
					q.z = (m[0][2] + m[2][0]) / s;
					q.w = (m[2][1] - m[1][2]) / s;
				} else if (m[1][1] > m[2][2]) {
					float s = 2.0f * std::sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]);
					q.x = (m[0][1] + m[1][0]) / s;
					q.y = 0.25f * s;
					q.z = (m[1][2] + m[2][1]) / s;
					q.w = (m[0][2] - m[2][0]) / s;
				} else {
					float s = 2.0f * std::sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]);
					q.x = (m[0][2] + m[2][0]) / s;
					q.y = (m[1][2] + m[2][1]) / s;
					q.z = -0.25f * s;
					q.w = (m[1][0] - m[0][1]) / s;
				}
			}

			return q.Normalized(); // 最後に必ず正規化する

		}

		static Quaternion FromLookAt(const Vector3& target, const Vector3& observer, const Vector3& above) {
			Vector3 forward = (target - observer).Normalized();

			Vector3 right = above.Cross(forward).Normalized();

			Vector3 up = forward.Cross(right);

			float trace = right.x + above.y + forward.z;
			Quaternion q{};

			if (trace > 0.0f) {
				float s = 2.0f * sqrtf(trace + 1.0f);
				q.x = (above.z - forward.y) / s;
				q.y = (forward.x - right.z) / s;
				q.z = -(right.y - above.x) / s;
				q.w = 0.25f * s;
			} else {
				// 対角成分が小さい場合の分岐処理 省略
			}

			return q.Normalized();
		}

		Vector3 RotateVector(const Vector3& vector) const {

			Quaternion p{ vector.x, vector.y, vector.z, 0 };

			Quaternion r =
				(*this) * p * this->Conjugated();

			return { r.x, r.y, r.z };

		}

		void AddRotation(const Quaternion& delta) {

			(*this) *= delta;

			this->Normalize();

		}

		static Quaternion FromTwoDirection(Vector3 from, Vector3 to) {

			from.Normalize();
			to.Normalize();

			constexpr Vector3 kRight = { 1.0f, 0.0f, 0.0f };

			constexpr Vector3 kUp = { 0.0f, 1.0f, 0.0f };

			float dot = from.Dot(to);

			if (dot < -0.99999f) {

				Vector3 axis = kRight.Cross(from);

				if (axis.Length() < 0.001f) {

					axis = kUp.Cross(from);

				}

				return Quaternion{ 0.0f, axis.x, axis.y, axis.z };

			}

			Vector3 axis = from.Cross(to);
			Quaternion q = { dot + 1.0f, axis.x, axis.y, -axis.z };

			return q.Normalized();

		}

		static Quaternion MakeRotateQuaternion(
			const Vector3& euler)
		{
			Quaternion qx =
				FromAxisAngle(
					{ 1,0,0 },
					euler.x);

			Quaternion qy =
				FromAxisAngle(
					{ 0,1,0 },
					euler.y);

			Quaternion qz =
				FromAxisAngle(
					{ 0,0,1 },
					euler.z);

			return (qx * qy * qz).Normalized();
		}

	};

}