#pragma once

#include "Math/Vector3.h"
#include <cmath>

namespace Atrum::Geometry {

// 球面座標系
struct SphericalCoordinate {
  // 動径
  float radius = 0.0f;
  // 天頂角
  float theta = 0.0f;
  // 方位角
  float phi = 0.0f;

  Math::Vector3 ToCartesian() const {
    return {radius * std::sin(theta) * std::cos(phi),
            radius * std::sin(theta) * std::sin(phi), radius * std::cos(theta)};
  }
};

using Spherical = SphericalCoordinate;

} // namespace Atrum::Geometry