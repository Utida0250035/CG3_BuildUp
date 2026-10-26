#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace Atrum {

/// カメラの view 行列から、画像面と平行なビルボードのワールド行列を作る
Math::Matrix4x4 MakeViewPlaneParallelBillboardWorld(
    const Math::Vector3 &worldPosition, const Math::Vector3 &worldScale,
    const Math::Matrix4x4 &viewMatrix);

} // namespace Atrum
