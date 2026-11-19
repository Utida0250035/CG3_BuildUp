#pragma once

#include <string>

namespace Atrum {

/// ムービーカメラトラック JSON を EditorAssets/Camera 基準で解決する
std::string ResolveMovieCameraTrackJsonPath(const std::string &path);

} // namespace Atrum