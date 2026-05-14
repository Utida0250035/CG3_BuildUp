#pragma once

#include "Vector2.h"

typedef struct OrientedBoundingBox {
	// 中心
	Vector2 center;
	// 各軸の長さ(幅, 高さ)
	Vector2 size;
	// 各軸の長さの半分
	Vector2 halfSize;
	// 正規化された方向ベクトル(ローカル右、ローカル上)
	Vector2 axis[2];

	Vector2 ToLocal(const Vector2& worldPos)const;

	void UpdateAxis(float theta);

	float CalculateAngle();

	bool IsPointInOBB(const Vector2& point) const {

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

} OBB;

struct RigidBodyOBB : public OBB {

	Vector2 velocity{};
	float angularVelocity = 0.0f;
	float mass = 1.0f;
	float inertiaMoment;

	void UpdateInertiaMoment();

};