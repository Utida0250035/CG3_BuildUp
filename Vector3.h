#pragma once

struct Vector3 {
	float x;
	float y;
	float z;
};

inline constexpr void operator+=(Vector3& me, const Vector3& other) {
	me.x += other.x;
	me.y += other.y;
	me.z += other.z;
}

inline constexpr Vector3 operator+(const Vector3& me, const Vector3& other) {

	return Vector3{ me.x + other.x, me.y + other.y, me.z + other.z };

}

inline constexpr void operator-=(Vector3& me, const Vector3& other) {
	me.x -= other.x;
	me.y -= other.y;
	me.z -= other.z;
}

inline constexpr Vector3 operator-(const Vector3& me, const Vector3& other) {

	return Vector3{ me.x - other.x, me.y - other.y, me.z - other.z };

}

inline constexpr void operator*=(Vector3& vector, const float& scalar) {
	vector.x *= scalar;
	vector.y *= scalar;
	vector.z *= scalar;
}

inline constexpr Vector3 operator*(const Vector3& vector, const float& scalar) {

	return Vector3{ vector.x * scalar, vector.y * scalar, vector.z * scalar };

}

inline constexpr Vector3 operator*(const float& scalar, const Vector3& vector) {

	return Vector3{ vector.x * scalar, vector.y * scalar, vector.z * scalar };

}

inline constexpr void operator/=(Vector3& vector, const float& scalar) {
	vector.x /= scalar;
	vector.y /= scalar;
	vector.z /= scalar;
}

inline constexpr Vector3 operator/(const Vector3& vector, const float& scalar) {

	return Vector3{ vector.x / scalar, vector.y / scalar, vector.z / scalar };

}

inline constexpr float VectorDot(const Vector3& me, const Vector3& other) {

	return me.x * other.x + me.y * other.y + me.z * other.z;

}

inline constexpr Vector3 VectorCross(const Vector3& me, const Vector3& other) {

	return Vector3{ me.y * other.z - me.z * other.y, me.z * other.x - me.x * other.z, me.x * other.y - me.y * other.x };

}

float VectorLength(const Vector3& vector);

inline constexpr float VectorLengthSquare(const Vector3& vector) {

	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;

}

inline Vector3 VectorNormalize(const Vector3& vector) {
	float length = VectorLength(vector);

	if (length > 0.00001f) {

		return vector / length;

	}

	return Vector3{ 0.0f, 0.0f, 0.0f };
}

static constexpr int kVectorPrintColumnWidth = 60;
static constexpr int kVectorPrintRowHeight = 20;

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);