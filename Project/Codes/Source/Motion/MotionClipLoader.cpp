#include "Motion/MotionClip.h"

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::Motion {

bool LoadMotionClipFromJson(const std::string &jsonPath, MotionClip &outClip) {
  outClip = {};

  std::ifstream file(jsonPath);
  if (!file.is_open()) {
    return false;
  }

  nlohmann::json root{};
  try {
    file >> root;
  } catch (const nlohmann::json::exception &) {
    return false;
  }

  outClip.name = root.value("name", std::string{});
  outClip.durationSeconds = root.value("duration", 0.0f);

  if (!root.contains("keyframes") || !root["keyframes"].is_array()) {
    return false;
  }

  for (const nlohmann::json &entry : root["keyframes"]) {
    MotionKeyframe keyframe{};
    keyframe.timeSeconds = entry.value("time", 0.0f);
    if (entry.contains("transform")) {
      keyframe.transform = entry.at("transform").get<Math::TransformLH>();
    }
    outClip.keyframes.push_back(keyframe);
  }

  if (outClip.keyframes.size() < 2) {
    return false;
  }

  std::sort(outClip.keyframes.begin(), outClip.keyframes.end(),
            [](const MotionKeyframe &a, const MotionKeyframe &b) {
              return a.timeSeconds < b.timeSeconds;
            });

  if (outClip.durationSeconds <= 0.0f) {
    outClip.durationSeconds = outClip.keyframes.back().timeSeconds;
  }

  return outClip.IsValid();
}

} // namespace Atrum::Motion
