#pragma once

#include "Math/Vector3.h"
#include <array>
#include <cstdint>

namespace Atrum::Renderer {

/// ワールド始点・終点から、カメラ向きの太さを持つ線分用クアッド（4 頂点）を組み立てる
/// @return 有効な線分なら true（長さ・幅が極小のとき false）
bool BuildCameraFacingLineQuad(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const float width, const Math::Vector3 &cameraWorldPosition,
    std::array<Math::Vector3, 4> &outPositions, Math::Vector3 &outNormal);

} // namespace Atrum::Renderer
