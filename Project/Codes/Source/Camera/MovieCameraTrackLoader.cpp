#include "Camera/MovieCameraTrackLoader.h"

#include "Camera/MovieCameraTrackKeyframeParsePolicy.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum {

bool LoadMovieCameraTrackFromJson(const std::string &jsonPath,
                                  std::vector<MovieCameraKeyframe> &outKeyframes) {
  outKeyframes.clear();

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

  if (!root.contains("keyframes") || !root["keyframes"].is_array()) {
    return false;
  }

  for (const nlohmann::json &entry : root["keyframes"]) {
    MovieCameraKeyframe keyframe{};
    if (!TryParseMovieCameraKeyframe(entry, keyframe)) {
      continue;
    }
    outKeyframes.push_back(keyframe);
  }

  return outKeyframes.size() >= 2;
}

} // namespace Atrum