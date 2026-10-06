#pragma once

#include "Math/Lerp.h"
#include <cassert>

namespace Atrum::Math {

	struct Vector3 {
		float x;
		float y;
		float z;

		Vector3& operator=(Vector3 other) {
			x = other.x;
			y = other.y;
			z = other.z;

			return (*this);

		}

		constexpr Vector3 operator-()const {

			return{ -x, -y, -z };

		}

		constexpr Vector3 operator+()const {

			return (*this);

		}

		constexpr Vector3& operator+=(const Vector3& other) {
			x += other.x;
			y += other.y;
			z += other.z;

			return (*this);

		}

		constexpr Vector3 operator+(const Vector3& other) const {

			Vector3 result = (*this);
			result += other;

			return result;

		}

		constexpr Vector3& operator-=(const Vector3& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;

			return (*this);

		}

		constexpr Vector3 operator-(const Vector3& other) const {

			Vector3 result = (*this);
			result -= other;

			return result;

		}

		constexpr Vector3& operator*=(const float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;

			return (*this);

		}

		constexpr Vector3 operator*(const float scalar) const {

			Vector3 result = (*this);

			result *= scalar;

			return result;

		}

		constexpr Vector3& operator/=(const float scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;

			return (*this);

		}

		constexpr Vector3 operator/(const float scalar) const {
			Vector3 result = (*this);
			result /= scalar;

			return result;
		}

		constexpr float Dot(const Vector3& other) const {

			return x * other.x + y * other.y + z * other.z;

		}

		constexpr Vector3 CrossLh(const Vector3& other) const {

			return Vector3{ y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };

		}

		constexpr Vector3 CrossRh(const Vector3& other) const {

			return other.CrossLh(*this);

		}

		float Length() const;

		constexpr float LengthSquare() const {

			return x * x + y * y + z * z;

		}

		void Normalize() {

			float length = Length();

			if (length <= 0.00001f || length <= 1.00001f && length >= 0.99999f) {

				return;

			}

			(*this) /= length;

		}

		[[nodiscard]] Vector3 Normalized() const {

			Vector3 normalized = (*this);

			normalized.Normalize();

			return normalized;

		}

		static constexpr Vector3 LeftLh() { return { -1.0f, 0.0f, 0.0f }; }
		static constexpr Vector3 LeftRh() { return -LeftLh(); }

		static constexpr Vector3 RightLh() { return { 1.0f, 0.0f, 0.0f }; }
		static constexpr Vector3 RightRh() { return -RightLh(); }

		static constexpr Vector3 DownLh() { return { 0.0f, -1.0f, 0.0f }; }
		static constexpr Vector3 DownRh() { return DownLh(); }

		static constexpr Vector3 UpLh() { return { 0.0f, 1.0f, 0.0f }; }
		static constexpr Vector3 UpRh() { return UpLh(); }

		static constexpr Vector3 BackLh() { return { 0.0f, 0.0f, -1.0f }; }
		static constexpr Vector3 BackRh() { return -BackLh(); }

		static constexpr Vector3 ForwardLh() { return { 0.0f, 0.0f, 1.0f }; }
		static constexpr Vector3 ForwardRh() { return -ForwardLh(); }

		static constexpr Vector3 Zero() { return { 0.0f, 0.0f, 0.0f }; }

		static constexpr Vector3 One() { return { 1.0f, 1.0f, 1.0f }; }

	};

	inline constexpr Vector3 operator*(const float& scalar, const Vector3& vector) {

		return Vector3{ vector.x * scalar, vector.y * scalar, vector.z * scalar };

	}

}

namespace Atrum::Interpolation {

	inline constexpr Math::Vector3 Lerp(const Math::Vector3& start, const Math::Vector3 end, const float t) {

		return (1 - t) * start + t * end;

	}

}

namespace Atrum::Physics {

	inline Math::Vector3 Cross(const Math::Vector3& me, const Math::Vector3& other) {

		return me.CrossRh(other);

	}

}