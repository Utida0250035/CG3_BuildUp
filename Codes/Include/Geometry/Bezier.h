#pragma once

#include "Math/Vector2.h"

namespace Atrum::Geometry {

	Vector2 CalcBezier2(const Vector2 p[3], float t);

	Vector2 CalcBezier2Tangent(const Vector2 p[3], float t);

}