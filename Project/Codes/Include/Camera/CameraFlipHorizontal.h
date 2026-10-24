#pragma once

#include "Camera/Camera.h"

namespace Atrum {

/// <summary>
/// カメラの左右を画面上映像上で反転する（視線ローカルの上軸まわり180度）
/// </summary>
void FlipCameraHorizontal(Camera &camera);

} // namespace Atrum
