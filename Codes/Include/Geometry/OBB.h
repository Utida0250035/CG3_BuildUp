#pragma once

#include "Math/Vector2.h"

namespace Atrum::Geometry {

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

		float CalculateAngle()const;

		bool IsPointInOBB(const Vector2& point) const;

		void GetWorldCorners(Vector2 corners[4]) const;

	} MyOBB;

	struct RigidBodyOBB : public MyOBB {

		Vector2 velocity{};
		float angularVelocity = 0.0f;
		float mass = 1.0f;
		float inertiaMoment;

		void UpdateInertiaMoment();

	};

}