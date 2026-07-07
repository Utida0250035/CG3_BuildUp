#pragma once
#include "Math/Vector3.h"

struct Plane {
	// 法線
	Vector3 normal;

	// 原点との法線方向の距離
	float distance;

};

Plane MakePlane(const Vector3& pointA, const Vector3& pointB, const Vector3& pointC);

inline Plane MakePlane(const Vector3 points[3]) {

	return MakePlane(points[0], points[1], points[2]);

}

float CalcDistance(const Plane& plane, const Vector3& point);