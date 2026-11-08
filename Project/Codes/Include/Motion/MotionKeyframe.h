#pragma once

#include "Math/Transform.h"

namespace Atrum::Motion {

struct MotionKeyframe {
  float timeSeconds = 0.0f;
  Math::TransformLH transform{};
};

} // namespace Atrum::Motion
