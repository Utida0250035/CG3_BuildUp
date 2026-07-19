#pragma once

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

		constexpr Vector3 Cross(const Vector3& other) const {

			return Vector3{ y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };

		}

		float Length() const;

		constexpr float LengthSquare() const {

			return x * x + y * y + z * z;

		}

		void Normalize() {

			float length = Length();

			if (length <= 0.00001f) {

				return;

			}

			(*this) /= length;

		}

		[[nodiscard]] Vector3 Normalized() const {
			Vector3 normalized = (*this);

			normalized.Normalize();

			return normalized;

		}

	};

	inline constexpr Vector3 operator*(const float& scalar, const Vector3& vector) {

		return Vector3{ vector.x * scalar, vector.y * scalar, vector.z * scalar };

	}

}