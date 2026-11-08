#pragma once

#include "Math/Matrix4x4.h"

namespace Atrum {

class AtrumEngine;

/// 地形・手前景に隠れた自機位置をシルエット球で示す（Depth Greater パス）
namespace PlayerTerrainSilhouette {

void DrawOccludedSilhouette(AtrumEngine *engine,
                            const Math::Matrix4x4 &playerWorldMatrix,
                            float sphereRadius);

} // namespace PlayerTerrainSilhouette

} // namespace Atrum
