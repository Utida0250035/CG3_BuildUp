#pragma once

#include "Camera/CameraController/MovieCameraController.h"

#include <nlohmann/json_fwd.hpp>

namespace Atrum {

/// 1 キーフレーム JSON を検証して out に書き込む。不正なら false（他キーフレームは呼び出し側で継続）。
bool TryParseMovieCameraKeyframe(const nlohmann::json &entry,
                                 MovieCameraKeyframe &outKeyframe);

} // namespace Atrum