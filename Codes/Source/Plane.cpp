#include "Plane.h"

Plane MakePlane(const Vector3& pointA, const Vector3& pointB, const Vector3& pointC) {

	Plane plane;

	Vector3 aToB = pointB - pointA;
	Vector3 btoC = pointC - pointB;

	plane.normal = VectorNormalize(VectorCross(aToB, btoC));

	plane.distance = VectorDot(pointA, plane.normal);

	return plane;

}

float CalcDistance(const Plane& plane, const Vector3& point) {

	return VectorDot(plane.normal, point) - plane.distance;

}