#pragma once
#include "Math/Lerp.h"
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

			return Quaternion{
				.x = 0.0f,
				.y = 0.0f,
				.z = 0.0f,
				.w = 1.0f
			};

		}

		[[nodiscard]] constexpr float MagnitudeSquare() const { return  w * w + x * x + y * y + z * z; }
		[[nodiscard]] float Magnitude() const { return std::sqrt(MagnitudeSquare()); }

		void Normalize() {
			float magnitude = this->Magnitude();

			if (magnitude > 0.0f) {

				if (magnitude >= 0.99999f && magnitude <= 1.00001f) {
					return;
				}

				x /= magnitude;
				y /= magnitude;
				z /= magnitude;
				w /= magnitude;

				return;

			}

			assert(false);

			*this = Identity();

		}

		[[nodiscard]] Quaternion Normalized() const {

			Quaternion q = *this;

			q.Normalize();

			return q;

		}

		[[nodiscard]] constexpr Quaternion operator-() const {

			return { .x = -x, .y = -y, .z = -z, .w = -w };

		}

		[[nodiscard]] constexpr Quaternion operator+() const {

			return (*this);

		}

		constexpr Quaternion& operator+=(const Quaternion& other) {

			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;

			return (*this);

		}

		[[nodiscard]] constexpr Quaternion operator+(const Quaternion& other) const {

			Quaternion result = (*this);

			result += other;

			return result;

		}

		constexpr Quaternion& operator-=(const Quaternion& other) {

			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;

			return (*this);

		}

		[[nodiscard]] constexpr Quaternion operator-(const Quaternion& other) const {

			Quaternion result = (*this);

			result -= other;

			return result;

		}

		constexpr Quaternion& operator*=(const float scalar) {

			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;

			return (*this);

		}

		[[nodiscard]] constexpr Quaternion operator*(const float scalar) const {

			Quaternion result = (*this);

			result *= scalar;

			return result;

		}

		constexpr Quaternion& operator*=(const Quaternion& other) {

			(*this) = {
				.x = w * other.x + x * other.w + y * other.z - z * other.y,

				.y = w * other.y + y * other.w + z * other.x - x * other.z,

				.z = w * other.z + z * other.w + x * other.y - y * other.x,

				.w = w * other.w - x * other.x - y * other.y - z * other.z
			};

			return (*this);

		}

		[[nodiscard]] constexpr Quaternion operator*(const Quaternion& other) const
		{

			Quaternion result = (*this);

			result *= other;

			return result;

		}

		[[nodiscard]] constexpr float Dot(const Quaternion& other) const {

			return x * other.x + y * other.y + z * other.z + w * other.w;

		}

		[[nodiscard]] float GetTheta() const {
			assert(std::abs(MagnitudeSquare() - 1.0f) < 0.001f);

			Quaternion nq = Normalized();

			float clamped_w = std::max(-1.0f, std::min(1.0f, nq.w));
			return 2.0f * std::acos(clamped_w);
		}

		constexpr void Conjugate() {
			x *= -1.0f;
			y *= -1.0f;
			z *= -1.0f;
		}

		[[nodiscard]] constexpr Quaternion Conjugated() const {
			return Quaternion(-x, -y, -z, w);
		}

		void LhZRemove() {

			Vector3 forward = this->RotateVector(Vector3::ForwardLh());

			Vector3 right = Vector3::UpLh().CrossLh(forward);

			if (right.LengthSquare() < 0.00001f) {
				return;
			}

			right.Normalize();

			Vector3 up = forward.CrossLh(right);
			up.Normalize();

			(*this) = Quaternion::FromAxes(right, up, forward);
			this->Normalize();

		}

		[[nodiscard]] Quaternion LhZRemoved() const {

			Quaternion result = (*this);

			result.LhZRemove();

			return result;

		}

		void RhZRemove() {

			Vector3 forward = this->RotateVector(Vector3::ForwardRh());

			Vector3 right = Vector3::UpRh().CrossRh(forward);

			if (right.LengthSquare() < 0.00001f) {
				return;
			}

			right.Normalize();

			Vector3 up = forward.CrossRh(right);
			up.Normalize();

			(*this) = Quaternion::FromAxes(right, up, forward);
			this->Normalize();

		}

		[[nodiscard]] Quaternion RhZRemoved() const {

			Quaternion result = (*this);

			result.RhZRemove();

			return result;

		}

		[[nodiscard]] Matrix4x4 MakeRotateMatrixRh() const {

			float x2 = x * x;
			float y2 = y * y;
			float z2 = z * z;
			float xy = x * y;
			float xz = x * z;
			float yz = y * z;
			float wx = w * x;
			float wy = w * y;
			float wz = w * z;

			Matrix4x4 rotateMatrix = Matrix4x4::Identity();

			// 行優先(Row-Major)で回転成分のみを上書き
			rotateMatrix[0][0] = 1.0f - 2.0f * (y2 + z2);
			rotateMatrix[0][1] = 2.0f * (xy - wz);
			rotateMatrix[0][2] = 2.0f * (xz + wy);

			rotateMatrix[1][0] = 2.0f * (xy + wz);
			rotateMatrix[1][1] = 1.0f - 2.0f * (x2 + z2);
			rotateMatrix[1][2] = 2.0f * (yz - wx);

			rotateMatrix[2][0] = 2.0f * (xz - wy);
			rotateMatrix[2][1] = 2.0f * (yz + wx);
			rotateMatrix[2][2] = 1.0f - 2.0f * (x2 + y2);

			return rotateMatrix;

		}

		[[nodiscard]] Matrix4x4 MakeRotateMatrixLh() const {

			float x2 = x * x;
			float y2 = y * y;
			float z2 = z * z;
			float xy = x * y;
			float xz = x * z;
			float yz = y * z;
			float wx = w * x;
			float wy = w * y;
			float wz = w * z;

			Matrix4x4 rotateMatrix = Matrix4x4::Identity();

			// 行優先(Row-Major)で回転成分のみを上書き
			rotateMatrix[0][0] = 1.0f - 2.0f * (y2 + z2);
			rotateMatrix[0][1] = 2.0f * (xy + wz);
			rotateMatrix[0][2] = 2.0f * (xz - wy);

			rotateMatrix[1][0] = 2.0f * (xy - wz);
			rotateMatrix[1][1] = 1.0f - 2.0f * (x2 + z2);
			rotateMatrix[1][2] = 2.0f * (yz + wx);

			rotateMatrix[2][0] = 2.0f * (xz + wy);
			rotateMatrix[2][1] = 2.0f * (yz - wx);
			rotateMatrix[2][2] = 1.0f - 2.0f * (x2 + y2);

			return rotateMatrix;

		}

		[[nodiscard]] static Quaternion FromAxisAngle(const Vector3& unitVector, const float angle) {

#ifdef _DEBUG

			float length = unitVector.Length();

			assert(length <= 1.00001f && length >= 0.99999f);

#endif

			float halfAngle = angle * 0.5f;
			float sin = std::sin(halfAngle);
			return Quaternion{ unitVector.x * sin, unitVector.y * sin, unitVector.z * sin, std::cos(halfAngle) };

		}

		[[nodiscard]] static Quaternion FromRotateMatrix(const Matrix4x4& rotateMatrix) {

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

			q.Normalize();

			return q;

		}

		[[nodiscard]] static Quaternion FromLhLookAt(const Vector3& target, const Vector3& eye, const Vector3& up) {
			return FromLhLookAt(target - eye, up);
		}

		[[nodiscard]] static Quaternion FromLhLookAt(const Vector3& forward, const Vector3& up) {
			Matrix4x4 lookAtMatrix = Matrix4x4::LhLookAt(forward, up);

			Quaternion lookAtQ = FromRotateMatrix(lookAtMatrix);

			lookAtQ.Normalize();

			return lookAtQ;

		}

		[[nodiscard]] static Quaternion FromRhLookAt(const Vector3& target, const Vector3& eye, const Vector3& up) {
			return FromRhLookAt(target - eye, up);
		}

		[[nodiscard]] static Quaternion FromRhLookAt(const Vector3& forward, const Vector3& up) {
			Matrix4x4 lookAtMatrix = Matrix4x4::RhLookAt(forward, up);

			Quaternion lookAtQ = FromRotateMatrix(lookAtMatrix);

			lookAtQ.Normalize();

			return lookAtQ;

		}

		[[nodiscard]] constexpr Vector3 RotateVector(const Vector3& vector) const {

			Quaternion p{ vector.x, vector.y, vector.z, 0.0f };

			Quaternion r =
				(*this) * p * this->Conjugated();

			Vector3 result = { r.x, r.y, r.z };

			return result;

		}

		[[nodiscard]] static Quaternion FromLhTwoDirection(Vector3 from, Vector3 to) {

			from.Normalize();
			to.Normalize();

			constexpr Vector3 kRight = { 1.0f, 0.0f, 0.0f };

			constexpr Vector3 kUp = { 0.0f, 1.0f, 0.0f };

			float dot = from.Dot(to);

			if (dot < -0.99999f) {

				Vector3 axis = kRight.CrossLh(from);

				if (axis.Length() < 0.001f) {

					axis = kUp.CrossLh(from);

				}

				return Quaternion{ axis.x, axis.y, axis.z, 0.0f };

			}

			Vector3 axis = from.CrossLh(to);
			Quaternion q = { axis.x, axis.y, axis.z, dot + 1.0f };

			return q.Normalized();

		}

		[[nodiscard]] static Quaternion FromRhTwoDirection(Vector3 from, Vector3 to) {

			from.Normalize();
			to.Normalize();

			constexpr Vector3 kRight = { 1.0f, 0.0f, 0.0f };

			constexpr Vector3 kUp = { 0.0f, 1.0f, 0.0f };

			float dot = from.Dot(to);

			if (dot < -0.99999f) {

				Vector3 axis = kRight.CrossRh(from);

				if (axis.Length() < 0.001f) {

					axis = kUp.CrossRh(from);

				}

				return Quaternion{ axis.x, axis.y, axis.z, 0.0f };

			}

			Vector3 axis = from.CrossRh(to);
			Quaternion q = { axis.x, axis.y, axis.z, dot + 1.0f };

			return q.Normalized();

		}

		[[nodiscard]] static Quaternion FromAxes(const Vector3& right, const Vector3& up, const Vector3& forward) {

			Vector3 r = right.Normalized();
			Vector3 u = up.Normalized();
			Vector3 f = forward.Normalized();

			Quaternion q;
			float trace = r.x + u.y + f.z;

			if (trace > 0.0f) {
				float s = std::sqrt(trace + 1.0f) * 2.0f; // s = 4 * w
				q.w = 0.25f * s;
				q.x = (u.z - f.y) / s;
				q.y = (f.x - r.z) / s;
				q.z = (r.y - u.x) / s;
			} else if ((r.x > u.y) && (r.x > f.z)) {
				float s = std::sqrt(1.0f + r.x - u.y - f.z) * 2.0f; // s = 4 * x
				q.w = (u.z - f.y) / s;
				q.x = 0.25f * s;
				q.y = (u.x + r.y) / s;
				q.z = (f.x + r.z) / s;
			} else if (u.y > f.z) {
				float s = std::sqrt(1.0f + u.y - r.x - f.z) * 2.0f; // s = 4 * y
				q.w = (f.x - r.z) / s;
				q.x = (u.x + r.y) / s;
				q.y = 0.25f * s;
				q.z = (f.y + u.z) / s;
			} else {
				float s = std::sqrt(1.0f + f.z - r.x - u.y) * 2.0f; // s = 4 * z
				q.w = (r.y - u.x) / s;
				q.x = (f.x + r.z) / s;
				q.y = (f.y + u.z) / s;
				q.z = 0.25f * s;
			}

			return q.Normalized();

		}

		[[nodiscard]] static Quaternion FromEulerXYZ(const Vector3& euler) {
			Quaternion qX =
				FromAxisAngle(
					{ 1,0,0 },
					euler.x);

			Quaternion qY =
				FromAxisAngle(
					{ 0,1,0 },
					euler.y);

			Quaternion qZ =
				FromAxisAngle(
					{ 0,0,1 },
					euler.z);

			return (qX * qY * qZ).Normalized();
		}

		[[nodiscard]] Vector3 ToEulerXYZ() const {
			// 回転角（X, Y, Z）
			float xAngle, yAngle, zAngle;

			// クォータニオンの成分
			float sqw = w * w;
			float sqx = x * x;
			float sqy = y * y;
			float sqz = z * z;

			// Y軸周りの回転 (sin(y)) を求める
			float sinY = 2.0f * (w * y - z * x);

			if (std::abs(sinY) >= 0.9999f) {
				// ジンバルロック時（Yが ±90度）
				yAngle = (sinY > 0) ? 1.570796f : -1.570796f; // ±π/2

				// この状態ではXとZの自由度が重なるため、片方を0固定にする
				xAngle = 0.0f;
				zAngle = std::atan2(2.0f * (x * y - w * z), 1.0f - 2.0f * (sqx + sqz));
			} else {
				yAngle = std::asin(std::clamp(sinY, -1.0f, 1.0f));

				// X軸周りの回転
				xAngle = std::atan2(2.0f * (y * z + w * x), sqw - sqx - sqy + sqz);

				// Z軸周りの回転
				zAngle = std::atan2(2.0f * (x * y + w * z), sqw + sqx - sqy - sqz);
			}

			return { xAngle, yAngle, zAngle };
		}

		[[nodiscard]] static Quaternion Lerp(const Quaternion& start, const Quaternion& end, const float t) {

			Quaternion result = start * (1.0f - t) + end * t;

			result.Normalize();

			return result;

		}

		[[nodiscard]] static Quaternion Slerp(const Quaternion& start, Quaternion end, const float t) {

			Quaternion result{};

			float dot = start.Dot(end);

			if (dot < 0.0f) {

				dot = -dot;

				end = -end;

			}

			if (dot > 0.9995f) {

				result = Lerp(start, end, t);

				return result;

			}

			float thetaO = std::acos(dot);

			float theta = thetaO * t;

			float sinTheta = std::sin(theta);
			float sinThetaO = std::sin(thetaO);

			float sO = std::cos(theta) - dot * sinTheta / sinThetaO;
			float s = sinTheta / sinThetaO;

			result = start * sO + end * s;

			result.Normalize();

			return result;

		}

	};

}

namespace Atrum::Interpolation {

	inline Math::Quaternion Lerp(const Math::Quaternion& start, const Math::Quaternion& end, const float t) {

		return Math::Quaternion::Lerp(start, end, t);

	}

}