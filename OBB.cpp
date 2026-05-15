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

float OBB::CalculateAngle()const {

	return std::atan2(axis[0].y, axis[0].x);

}

bool OBB::IsPointInOBB(const Vector2& point) const {

	// 各軸（axis[0], axis[1]）に投影してローカル座標を求める
	Vector2 localPoint = ToLocal(point);

	if (std::abs(localPoint.x) <= halfSize.x) {

		if (std::abs(localPoint.y) <= halfSize.y) {
			// ローカル座標の絶対値が halfSize 以内なら内部

			return true;

		}

	}

	return false;

}

void OBB::GetWorldCorners(Vector2 corners[4]) const {

	Vector2 offsetX = axis[0] * halfSize.x;

	Vector2 offsetY = axis[1] * halfSize.y;

	corners[0] = center + offsetX + offsetY;

	corners[1] = center - offsetX + offsetY;
	
	corners[2] = center - offsetX - offsetY;

	corners[3] = center + offsetX - offsetY;

}

void RigidBodyOBB::UpdateInertiaMoment() {

	inertiaMoment = (1.0f / 12.0f) * mass * VectorLengthSquare(size);

}