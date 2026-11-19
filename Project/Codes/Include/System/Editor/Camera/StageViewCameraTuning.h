#pragma once

#include "Camera/StageViewFramingBounds.h"
#include "Camera/StageViewCamera.h"

#include <cstdint>
#include <string>

namespace Atrum::Editor {

/// DEVELOPMENT: stage_view_camera_tuning.json の読込・ホットリロード・Game ビュー試写
class StageViewCameraTuning final {
public:
  static StageViewCameraTuning &Instance();

  bool IsGameViewPreviewEnabled() const { return gameViewPreviewEnabled_; }

  void SetGameViewPreviewEnabled(const bool enabled) {
    gameViewPreviewEnabled_ = enabled;
  }

  bool IsAutoHotReloadEnabled() const { return autoHotReload_; }
  void SetAutoHotReloadEnabled(const bool enabled) { autoHotReload_ = enabled; }

  const std::string &JsonPath() const { return jsonPath_; }
  void SetJsonPath(std::string path);

  bool ReloadFromDisk();
  void PollHotReload();

  bool IsLoaded() const { return loaded_; }
  const std::string &LastLoadMessage() const { return lastLoadMessage_; }

  StageViewCameraPose ComputeBattlePose(
      const StageViewFramingBounds &framingBounds) const;

  float PerspectiveFovY() const { return perspectiveFovY_; }
  float AspectRatio() const { return aspectRatio_; }

private:
  StageViewCameraTuning() = default;

  bool TryParseFile(const std::string &path);

  std::string jsonPath_{};
  std::string lastLoadMessage_{};
  std::int64_t lastWriteTimeNs_ = 0;

  float perspectiveFovY_ = 0.5f;
  float aspectRatio_ = 1280.0f / 720.0f;
  float battleDistanceScale_ = 1.15f;
  float battleEyeHeightAbovePivotY_ = 3.6f;
  float battleEyeSideOffsetX_ = -2.25f;
  float battleElevationFitFactor_ = 0.12f;

  bool gameViewPreviewEnabled_ = false;
  bool autoHotReload_ = true;
  bool loaded_ = false;
};

void DrawStageViewCameraTuningPanel();

} // namespace Atrum::Editor