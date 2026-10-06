#pragma once

#include "Math/Vector2.h"

namespace Atrum::Geometry::Bezier {

	Math::Vector2 CalcBezier2(const Math::Vector2 p[3], float t);

	Math::Vector2 CalcBezier2Tangent(const Math::Vector2 p[3], float t);

}