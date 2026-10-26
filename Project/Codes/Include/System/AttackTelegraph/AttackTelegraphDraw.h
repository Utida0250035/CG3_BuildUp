#pragma once

#include "System/AttackTelegraph/AttackTelegraphSegment.h"

#include <vector>

namespace Atrum {

/// 予測線の見た目（全ボス攻撃テレグラフ共通）
inline constexpr float kAttackTelegraphLineWidth = 0.19f;
inline constexpr float kAttackTelegraphDashLength = 0.42f;
inline constexpr float kAttackTelegraphDashGap = 0.28f;

void DrawAttackTelegraphSegments(
    const std::vector<AttackTelegraphSegment> &segments);

} // namespace Atrum
