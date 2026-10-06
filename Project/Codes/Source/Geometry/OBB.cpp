#include "Engine/Alias/GeometryAlias.h"
#include "Geometry/OBB.h"
#include <cmath>

namespace Atrum::Geometry {

	M::Vector2 OrientedBoundingBox::ToLocal(const M::Vector2& worldPos) const {

		M::Vector2 relation = worldPos - center;
		return { relation.Dot(axis[0]), relation.Dot(axis[1]) };

	}

	void OrientedBoundingBox::UpdateAxis(float theta) {

		// 右方向軸
		axis[0] = M::Vector2{ std::cos(theta), std::sin(theta) };

		// 上方向軸 右方向軸を90°回転
		axis[1] = { -axis[0].y, axis[0].x };

	}

	float OrientedBoundingBox::CalculateAngle()const {

		return std::atan2(axis[0].y, axis[0].x);

	}

	bool OrientedBoundingBox::IsPointInOBB(const M::Vector2& point) const {

		// 各軸（axis[0], axis[1]）に投影してローカル座標を求める
		M::Vector2 localPoint = ToLocal(point);

		if (std::abs(localPoint.x) <= halfSize.x) {

			if (std::abs(localPoint.y) <= halfSize.y) {
				// ローカル座標の絶対値が halfSize 以内なら内部

				return true;

			}

		}

		return false;

	}

	void OrientedBoundingBox::GetWorldCorners(M::Vector2 corners[4]) const {

		M::Vector2 offsetX = axis[0] * halfSize.x;

		M::Vector2 offsetY = axis[1] * halfSize.y;

		corners[0] = center + offsetX + offsetY;

		corners[1] = center - offsetX + offsetY;

		corners[2] = center - offsetX - offsetY;

		corners[3] = center + offsetX - offsetY;

	}

}

namespace Atrum::Physics {

	void RigidBodyOBB::UpdateInertiaMoment() {

		inertiaMoment = (1.0f / 12.0f) * mass * size.LengthSquare();

	}

}