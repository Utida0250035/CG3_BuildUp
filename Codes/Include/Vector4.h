#pragma once
#include <cmath>

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

inline constexpr void operator+=(Vector4& me, const Vector4& other) {
	me.x += other.x;
	me.y += other.y;
	me.z += other.z;
	me.w += other.w;
}

inline constexpr Vector4 operator+(const Vector4& me, const Vector4& other) {

	return Vector4{ me.x + other.x, me.y + other.y, me.z + other.z, me.w + other.w };

}

inline constexpr void operator-=(Vector4& me, const Vector4& other) {
	me.x -= other.x;
	me.y -= other.y;
	me.z -= other.z;
	me.w -= other.w;
}

inline constexpr Vector4 operator-(const Vector4& me, const Vector4& other) {

	return Vector4{ me.x - other.x, me.y - other.y, me.z - other.z, me.w - other.w };

}

inline constexpr void operator*=(Vector4& vector, const float& scalar) {
	vector.x *= scalar;
	vector.y *= scalar;
	vector.z *= scalar;
	vector.w *= scalar;
}

inline constexpr Vector4 operator*(const Vector4& vector, const float& scalar) {

	return Vector4{ vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar };

}

inline constexpr Vector4 operator*(const float& scalar, const Vector4& vector) {

	return Vector4{ vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar };

}

inline constexpr void operator/=(Vector4& vector, const float& scalar) {
	vector.x /= scalar;
	vector.y /= scalar;
	vector.z /= scalar;
	vector.w /= scalar;
}

inline constexpr Vector4 operator/(const Vector4& vector, const float& scalar) {

	return Vector4{ vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w / scalar };

}

inline constexpr float VectorDot(const Vector4& me, const Vector4& other) {

	return me.x * other.x + me.y * other.y + me.z * other.z + me.w * other.w;

}

float VectorLength(const Vector4& vector);

inline constexpr float VectorLengthSquare(const Vector4& vector) {

	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;

}

inline Vector4 VectorNormalize(const Vector4& vector) {
	float length = VectorLength(vector);

	if (length > 0.00001f) {

		return vector / length;

	}

	return Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };

}

inline constexpr Vector4 Vec4White() {

	return Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };

}

inline constexpr Vector4 Vec4BLACK() {

	return Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };

}