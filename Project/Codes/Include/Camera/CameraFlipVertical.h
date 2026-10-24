#pragma once

#include "Camera/Camera.h"

namespace Atrum {

/// <summary>
/// カメラの上下を画面上映像上で反転する（視線ローカルの右軸まわり180度）
/// </summary>
void FlipCameraVertical(Camera &camera);

} // namespace Atrum
