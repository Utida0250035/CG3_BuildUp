#pragma once

#include "Math/Vector3.h"

namespace Atrum {

class Camera;

namespace Player {

/// スクリーン座標からゲームプレイ平面（水平 Y=planeY）上の点を求める
bool TryScreenPointToHorizontalPlaneY(const Camera &camera, float screenX,
                                    float screenY, float viewWidth,
                                    float viewHeight, float planeY,
                                    Math::Vector3 &outWorldPoint);

} // namespace Player
} // namespace Atrum