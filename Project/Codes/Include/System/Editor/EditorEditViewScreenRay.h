#pragma once

#include "Math/Vector3.h"

namespace Atrum {

class Camera;

} // namespace Atrum

namespace Atrum::Editor {

/// エディットビュー上のスクリーン座標から、水平面 Y=planeY との交点を求める。
bool TryPickHorizontalPlaneY(const ::Atrum::Camera &camera, float localX,
                             float localY, float viewWidth, float viewHeight,
                             float planeY, Math::Vector3 &outHit);

} // namespace Atrum::Editor