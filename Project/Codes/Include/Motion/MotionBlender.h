#pragma once

#include "Motion/MotionClip.h"
#include "Math/Transform.h"

namespace Atrum::Motion {

/// クリップ内キーフレームを時間サンプル
Math::TransformLH SampleMotionClip(const MotionClip &clip, float timeSeconds);

/// 2 モーションを weight でブレンド（0 = from のみ, 1 = to のみ）
Math::TransformLH BlendMotionTransforms(const Math::TransformLH &from,
                                        const Math::TransformLH &to,
                                        float blendWeight01);

} // namespace Atrum::Motion
