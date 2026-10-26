#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace Atrum::Renderer {

/// カメラの view 行列から、画像面と平行なビルボードのワールド行列を作る
Atrum::Math::Matrix4x4 MakeViewPlaneParallelBillboardWorld(
    const Atrum::Math::Vector3 &worldPosition,
    const Atrum::Math::Vector3 &worldScale,
    const Atrum::Math::Matrix4x4 &viewMatrix);

} // namespace Atrum::Renderer
