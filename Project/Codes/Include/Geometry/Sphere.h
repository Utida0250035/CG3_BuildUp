#pragma once

#include "Math/Vector3.h"

namespace Atrum::Geometry {

struct Sphere {
  Math::Vector3 center{};
  float radius = 0.0f;
};

} // namespace Atrum::Geometry