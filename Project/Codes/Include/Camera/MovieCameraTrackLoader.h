#pragma once

#include "Camera/CameraController/MovieCameraController.h"

#include <string>
#include <vector>

namespace Atrum {

/// JSON からムービーカメラキーフレームを読み込む（不正キーフレームはスキップ、有効が 2 未満なら false）
bool LoadMovieCameraTrackFromJson(const std::string &jsonPath,
                                  std::vector<MovieCameraKeyframe> &outKeyframes);

} // namespace Atrum
