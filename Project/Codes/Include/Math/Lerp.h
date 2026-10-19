#pragma once

#include <cmath>
#include <concepts>

namespace Atrum::Math {

template <typename T = float>
  requires std::is_floating_point_v<T>
inline constexpr T Lerp(const T startValue, const T endValue, const T t) {
  return std::lerp(startValue, endValue, t);
}

} // namespace Atrum::Math