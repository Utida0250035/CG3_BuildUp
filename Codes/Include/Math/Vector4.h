#pragma once
#include <cmath>

namespace Atrum::Math {

	struct Vector4 {
		float x;
		float y;
		float z;
		float w;

		constexpr Vector4& operator+=(const Vector4& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;

			return (*this);

		}

		constexpr Vector4 operator+(const Vector4& other) const {

			Vector4 result = (*this);
			result += other;

			return result;

		}

		constexpr Vector4& operator-=(const Vector4& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;

			return (*this);

		}

		constexpr Vector4 operator-(const Vector4& other) const {
			Vector4 result = (*this);
			result -= other;

			return result;
		}

		constexpr Vector4& operator*=(const float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;

			return (*this);

		}

		constexpr Vector4 operator*(const float scalar) const {
			Vector4 result = (*this);
			result *= scalar;

			return result;
		}

		constexpr Vector4& operator/=(const float scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
			w /= scalar;

			return (*this);

		}

		constexpr Vector4 operator/(const float scalar) const {

			Vector4 result = (*this);
			result /= scalar;

			return result;

		}


		constexpr float Dot(const Vector4& other) const {

			return x * other.x + y * other.y + z * other.z + w * other.w;

		}

		float Length() const;

		inline constexpr float LengthSquare() const {

			return x * x + y * y + z * z + w * w;

		}

		Vector4 Normalized() const {
			float length = Length();

			if (length > 0.00001f) {

				return (*this) / length;

			}

			return Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };

		}

		static constexpr Vector4 White() {

			return Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };

		}

		static constexpr Vector4 Red() {

			return Vector4{ 1.0f, 0.0f, 0.0f, 1.0f };

		}

		static constexpr Vector4 Black() {

			return Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };

		}


	};

	inline constexpr Vector4 operator*(const float scalar, const Vector4& vector) {

		return Vector4{ vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar };

	}

}