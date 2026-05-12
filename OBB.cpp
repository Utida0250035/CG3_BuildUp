#include "OBB.h"
#include <cmath>

Vector2 OBB::ToLocal(const Vector2& worldPos) const {

	Vector2 relation = worldPos - center;
	return { VectorDot(relation, axis[0]), VectorDot(relation, axis[1]) };

}

void OBB::UpdateAxis(float theta) {

	// 右方向軸
	axis[0] = Vector2{ std::cos(theta), std::sin(theta) };

	// 上方向軸 右方向軸を90°回転
	axis[1] = { -axis[0].y, axis[0].x };

}

float OBB::CalculateAngle() {

	return std::atan2(axis[0].y, axis[0].x);

}

void RigidBodyOBB::CalculateInertiaMoment() {

	inertiaMoment = (1.0f / 12.0f) * mass * VectorLengthSquare(size);

}