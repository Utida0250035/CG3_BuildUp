#include "Bezier.h"

Vector2 CalcBezier2(const Vector2 p[3], float t) {

	float u = 1.0f - t;
	return p[0] * (u * u) + p[1] * (2.0f * u * t) + p[2] * (t * t);

}

Vector2 CalcBezier2Tangent(const Vector2 p[3], float t) {

	return (p[1] - p[0]) * (2.0f * (1.0f - t)) + (p[2] - p[1]) * (2.0f * t);

}