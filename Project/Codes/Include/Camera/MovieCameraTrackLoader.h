#pragma once

#include "Camera/CameraController/MovieCameraController.h"

#include <string>
#include <vector>

namespace Atrum {

/// JSON からムービーカメラキーフレームを読み込む（失敗時は false）
bool LoadMovieCameraTrackFromJson(const std::string &jsonPath,
                                  std::vector<MovieCameraKeyframe> &outKeyframes);

} // namespace Atrum
