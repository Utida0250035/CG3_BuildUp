#include "System/Editor/Camera/StageViewCameraTuning.h"

#include "System/Editor/EditorAssetsPaths.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::Editor {

namespace {

StageViewCameraPose ComputeViewPoseTuned(
    const Math::Vector3 &pivot, const float viewHalfHeight,
    const float viewHalfWidth, const float distanceScale,
    const float eyeHeightAbovePivotY, const float eyeSideOffsetX,
    const float perspectiveFovY, const float aspectRatio,
    const float elevationFitFactor) {
  const float halfTan = std::tan(perspectiveFovY * 0.5f);
  const float distanceForHeight = viewHalfHeight / halfTan;
  const float distanceForWidth = viewHalfWidth / (halfTan * aspectRatio);
  const float eyeBackFromStage =
      std::max(distanceForHeight, distanceForWidth) * distanceScale;

  const float elevationFitScale =
      1.0f + std::max(eyeHeightAbovePivotY, 0.0f) /
                 std::max(viewHalfHeight, 1.0e-3f) * elevationFitFactor;
  const float adjustedBack = eyeBackFromStage * elevationFitScale;

  StageViewCameraPose pose{};
  pose.pivot = pivot;
  pose.eye = {pivot.x + eyeSideOffsetX,
              pivot.y + eyeHeightAbovePivotY,
              pivot.z - adjustedBack};
  return pose;
}

std::int64_t FileWriteTimeNs(const std::filesystem::path &path) {
  namespace fs = std::filesystem;
  std::error_code error;
  const fs::file_time_type writeTime = fs::last_write_time(path, error);
  if (error) {
    return 0;
  }
  return static_cast<std::int64_t>(
      writeTime.time_since_epoch().count());
}

} // namespace

StageViewCameraTuning &StageViewCameraTuning::Instance() {
  static StageViewCameraTuning instance;
  return instance;
}

void StageViewCameraTuning::SetJsonPath(std::string path) {
  jsonPath_ = std::move(path);
  ReloadFromDisk();
}

bool StageViewCameraTuning::ReloadFromDisk() {
  if (jsonPath_.empty()) {
    jsonPath_ = EditorAssets::kStageViewCameraTuningJson;
  }
  const bool ok = TryParseFile(jsonPath_);
  lastLoadMessage_ = ok ? "loaded: " + jsonPath_ : "load failed: " + jsonPath_;
  return ok;
}

void StageViewCameraTuning::PollHotReload() {
  if (!autoHotReload_ || jsonPath_.empty()) {
    return;
  }

  namespace fs = std::filesystem;
  std::error_code error;
  if (!fs::exists(jsonPath_, error)) {
    return;
  }

  const std::int64_t writeTime = FileWriteTimeNs(fs::path(jsonPath_));
  if (writeTime <= 0 || writeTime == lastWriteTimeNs_) {
    return;
  }
  TryParseFile(jsonPath_);
}

bool StageViewCameraTuning::TryParseFile(const std::string &path) {
  namespace fs = std::filesystem;
  if (!fs::exists(path)) {
    loaded_ = false;
    return false;
  }

  std::ifstream file(path);
  if (!file.is_open()) {
    loaded_ = false;
    return false;
  }

  nlohmann::json root;
  try {
    file >> root;
  } catch (const nlohmann::json::exception &) {
    loaded_ = false;
    return false;
  }

  perspectiveFovY_ = root.value("perspectiveFovY", perspectiveFovY_);
  const int clientWidth = root.value("clientWidth", 1280);
  const int clientHeight = root.value("clientHeight", 720);
  aspectRatio_ = static_cast<float>(clientWidth) /
                 static_cast<float>(std::max(clientHeight, 1));

  if (root.contains("stageBattle") && root.at("stageBattle").is_object()) {
    const nlohmann::json &battle = root.at("stageBattle");
    battleDistanceScale_ = battle.value("distanceScale", battleDistanceScale_);
    battleEyeHeightAbovePivotY_ =
        battle.value("eyeHeightAbovePivotY", battleEyeHeightAbovePivotY_);
    battleEyeSideOffsetX_ =
        battle.value("eyeSideOffsetX", battleEyeSideOffsetX_);
    battleElevationFitFactor_ =
        battle.value("elevationFitFactor", battleElevationFitFactor_);
  }

  loaded_ = true;
  lastWriteTimeNs_ = FileWriteTimeNs(fs::path(path));
  lastLoadMessage_ = "loaded: " + path;
  return true;
}

StageViewCameraPose StageViewCameraTuning::ComputeBattlePose(
    const StageViewFramingBounds &framingBounds) const {
  if (!loaded_) {
    return ComputeStageBattleViewPose(framingBounds);
  }

  const float centerX =
      (framingBounds.leftEndX + framingBounds.rightEndX) * 0.5f;
  const float bottomY = framingBounds.groundHeight;
  const float topY = framingBounds.barrageSpawnTopY;
  const float centerY = (bottomY + topY) * 0.5f;
  const float viewHeight = (topY - bottomY) * 0.5f;
  const float viewWidth =
      (framingBounds.rightEndX - framingBounds.leftEndX) * 0.5f;

  const Math::Vector3 pivot{centerX, centerY, framingBounds.pivotZ};
  return ComputeViewPoseTuned(pivot, viewHeight, viewWidth, battleDistanceScale_,
                              battleEyeHeightAbovePivotY_,
                              battleEyeSideOffsetX_, perspectiveFovY_,
                              aspectRatio_, battleElevationFitFactor_);
}

} // namespace Atrum::Editor