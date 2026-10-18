#pragma once

#include "Math/Vector3.h"

namespace Atrum::Geometry {

typedef struct AxisAlignedBoundingBox {
  Math::Vector3 min{};
  Math::Vector3 max{};
} AABB;

} // namespace Atrum::Geometry