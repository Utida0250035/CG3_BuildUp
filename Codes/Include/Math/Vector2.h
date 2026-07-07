#pragma once
#include <cmath>

struct Vector2 {
	float x;
	float y;
};

float VectorLength(const Vector2& me);

inline constexpr float VectorLengthSquare(const Vector2& me) {

	return me.x * me.x + me.y * me.y;

}

inline Vector2 VectorNormalize(const Vector2& me) {

	float length = VectorLength(me);

	if (length == 0.0f) {

		return { 0.0, 0.0f };

	} else {

		return { me.x / length, me.y / length };

	}

}

inline constexpr float VectorDot(const Vector2& me, const Vector2& other) {

	return me.x * other.x + me.y * other.y;

}

inline constexpr float VectorCross(const Vector2& me, const Vector2& other) {

	return me.x * other.y - me.y * other.x;

}

inline constexpr Vector2 operator+(const Vector2& me, const Vector2& other) {

	return { me.x + other.x, me.y + other.y };

}

inline constexpr void operator+=(Vector2& me, const Vector2& other) {

	me.x += other.x;
	me.y += other.y;

}

inline constexpr Vector2 operator-(const Vector2& me, const Vector2& other) {

	return { me.x - other.x, me.y - other.y };

}

inline constexpr void operator-=(Vector2& me, const Vector2& other) {

	me.x -= other.x;
	me.y -= other.y;

}

inline constexpr Vector2 operator*(const Vector2& me, const float& scalar) {

	return { me.x * scalar, me.y * scalar };

}

inline constexpr Vector2 operator*(const float& scalar, const Vector2& vector) {

	return { scalar * vector.x, scalar * vector.y };

}

inline constexpr void operator*=(Vector2& me, const float& scalar) {

	me.x *= scalar;
	me.y *= scalar;

}

inline constexpr Vector2 operator/(const Vector2& me, const float& scalar) {

	return { me.x / scalar, me.y / scalar };

}

inline constexpr void operator/=(Vector2& me, const float& scalar) {

	me.x /= scalar;
	me.y /= scalar;

}