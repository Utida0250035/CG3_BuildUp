#include "Engine/Alias/GeometryAlias.h"

#include "Geometry/Plane.h"

namespace Atrum::Geometry {

	Plane MakePlane(const M::Vector3& pointA, const M::Vector3& pointB, const M::Vector3& pointC) {

		Plane plane{};

		M::Vector3 aToB = pointB - pointA;
		M::Vector3 btoC = pointC - pointB;

		plane.normal = aToB.Cross(btoC).Normalized();

		plane.distance = pointA.Dot(plane.normal);

		return plane;

	}

	float CalcDistance(const Plane& plane, const M::Vector3& point) {

		return plane.normal.Dot(point) - plane.distance;

	}

}