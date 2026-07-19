#pragma once
#include <cmath>

namespace Atrum::Math {

	struct Vector2 {
		float x;
		float y;

		constexpr void operator+=(const Vector2& other) {

			x += other.x;
			y += other.y;

		}

		constexpr Vector2 operator+(const Vector2& other) const {

			Vector2 result = (*this);
			result += other;

			return result;

		}

		constexpr Vector2& operator-=(const Vector2& other) {

			x -= other.x;
			y -= other.y;

			return (*this);

		}

		inline constexpr Vector2 operator-(const Vector2& other) const {

			Vector2 result = (*this);
			result -= other;

			return result;

		}

		inline constexpr Vector2& operator*=(const float scalar) {

			x *= scalar;
			y *= scalar;

			return (*this);

		}

		inline constexpr Vector2 operator*(const float scalar) const {

			Vector2 result = (*this);
			result *= scalar;

			return result;

		}

		inline constexpr Vector2& operator/=(const float scalar) {

			x /= scalar;
			y /= scalar;

			return (*this);

		}

		inline constexpr Vector2 operator/(const float scalar) const {

			Vector2 result = (*this);
			result /= scalar;

			return result;

		}


		float Length() const;

		constexpr float LengthSquare() const {

			return x * x + y * y;

		}

		void Normalize() {

			float length = Length();

			if (length == 0.0f) {

				return;

			}

			(*this) /= length;

		}

		[[nodiscard]]Vector2 Normalized() const {

			Vector2 normalized = (*this);

			normalized.Normalize();

			return normalized;

		}

		constexpr float Dot(const Vector2& other) const {

			return x * other.x + y * other.y;

		}

		constexpr float Cross(const Vector2& other) const {

			return x * other.y - y * other.x;

		}

	};

	inline constexpr Vector2 operator*(const float scalar, const Vector2& vector) {

		return { scalar * vector.x, scalar * vector.y };

	}

}