#pragma once
#include "Math/Vector3.h"

namespace Atrum::Geometry {

	struct Plane {
		// 法線
		Math::Vector3 normal;

		// 原点との法線方向の距離
		float distance;

	};

	Plane MakePlane(const Math::Vector3& pointA, const Math::Vector3& pointB, const Math::Vector3& pointC);

	inline Plane MakePlane(const Math::Vector3 points[3]) {

		return MakePlane(points[0], points[1], points[2]);

	}

	float CalcDistance(const Plane& plane, const Math::Vector3& point);

}