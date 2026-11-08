#include "Camera/MovieCameraTrackLoader.h"

#include "Math/Vector3.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum {

namespace {

StageViewCameraEaseKind ParseEaseKind(const std::string &name) {
  if (name == "FocusPush") {
    return StageViewCameraEaseKind::FocusPush;
  }
  if (name == "LegacyInOutQuad") {
    return StageViewCameraEaseKind::LegacyInOutQuad;
  }
  return StageViewCameraEaseKind::PullBackReveal;
}

bool ParsePose(const nlohmann::json &j, StageViewCameraPose &outPose) {
  if (!j.contains("eye") || !j.contains("pivot")) {
    return false;
  }

  outPose.eye = j.at("eye").get<Math::Vector3>();
  outPose.pivot = j.at("pivot").get<Math::Vector3>();
  return true;
}

} // namespace

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
    keyframe.timeSeconds = entry.value("time", 0.0f);

    if (!ParsePose(entry, keyframe.pose)) {
      return false;
    }

    if (entry.contains("easeToNext")) {
      keyframe.easeToNext = ParseEaseKind(entry["easeToNext"].get<std::string>());
    }

    outKeyframes.push_back(keyframe);
  }

  return outKeyframes.size() >= 2;
}

} // namespace Atrum
