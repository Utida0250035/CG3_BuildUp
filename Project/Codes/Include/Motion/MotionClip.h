#pragma once

#include "Motion/MotionKeyframe.h"

#include <string>
#include <vector>

namespace Atrum::Motion {

struct MotionClip {
  std::string name{};
  float durationSeconds = 0.0f;
  std::vector<MotionKeyframe> keyframes{};

  bool IsValid() const { return keyframes.size() >= 2 && durationSeconds > 0.0f; }
};

bool LoadMotionClipFromJson(const std::string &jsonPath, MotionClip &outClip);

} // namespace Atrum::Motion
