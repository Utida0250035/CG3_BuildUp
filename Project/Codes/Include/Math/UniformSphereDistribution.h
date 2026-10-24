#pragma once

#include "Math/Vector3.h"

#include <cmath>

namespace Atrum::Math {

/// 球面上に均等分布した点（インデックスベース、決定的）
inline Vector3 SampleUniformSpherePoint(const int index, const int count,
                                        const float radius) {
  if (count <= 0) {
    return {};
  }

  const float normalizedIndex =
      (static_cast<float>(index) + 0.5f) / static_cast<float>(count);
  const float y = 1.0f - normalizedIndex * 2.0f;
  const float ringRadius = std::sqrt(std::max(0.0f, 1.0f - y * y));
  const float goldenAngle = 2.39996323f;
  const float phi = static_cast<float>(index) * goldenAngle;

  return {std::cos(phi) * ringRadius * radius, y * radius,
          std::sin(phi) * ringRadius * radius};
}

} // namespace Atrum::Math
