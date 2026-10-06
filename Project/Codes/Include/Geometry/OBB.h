#pragma once

#include "Math/Vector2.h"

namespace Atrum::Geometry {

	typedef struct OrientedBoundingBox {
		// 中心
		Math::Vector2 center;
		// 各軸の長さ(幅, 高さ)
		Math::Vector2 size;
		// 各軸の長さの半分
		Math::Vector2 halfSize;
		// 正規化された方向ベクトル(ローカル右、ローカル上)
		Math::Vector2 axis[2];

		Math::Vector2 ToLocal(const Math::Vector2& worldPos)const;

		void UpdateAxis(float theta);

		float CalculateAngle()const;

		bool IsPointInOBB(const Math::Vector2& point) const;

		void GetWorldCorners(Math::Vector2 corners[4]) const;

	} MyOBB;

}

namespace Atrum::Physics {

	struct RigidBodyOBB : public Geometry::MyOBB {

		Math::Vector2 velocity{};
		float angularVelocity = 0.0f;
		float mass = 1.0f;
		float inertiaMoment;

		void UpdateInertiaMoment();

	};

}