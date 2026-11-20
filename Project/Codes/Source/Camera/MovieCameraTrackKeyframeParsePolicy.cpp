#include "Camera/MovieCameraTrackKeyframeParsePolicy.h"

#include "Math/Vector3.h"

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

} // namespace

bool TryParseMovieCameraKeyframe(const nlohmann::json &entry,
                                 MovieCameraKeyframe &outKeyframe) {
  if (!entry.is_object()) {
    return false;
  }
  if (!entry.contains("eye") || !entry.contains("pivot")) {
    return false;
  }

  try {
    MovieCameraKeyframe keyframe{};
    keyframe.timeSeconds = entry.value("time", 0.0f);
    keyframe.pose.eye = entry.at("eye").get<Math::Vector3>();
    keyframe.pose.pivot = entry.at("pivot").get<Math::Vector3>();

    if (entry.contains("easeToNext") && entry.at("easeToNext").is_string()) {
      keyframe.easeToNext =
          ParseEaseKind(entry.at("easeToNext").get<std::string>());
    }

    outKeyframe = keyframe;
    return true;
  } catch (const nlohmann::json::exception &) {
    return false;
  }
}

} // namespace Atrum