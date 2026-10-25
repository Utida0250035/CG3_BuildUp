#pragma once

#include "System/AttackTelegraph/AttackTelegraphSegment.h"

#include <vector>

namespace Atrum {

/// 予測線の見た目（全ボス攻撃テレグラフ共通）
inline constexpr float kAttackTelegraphLineSphereRadius = 0.095f;
inline constexpr float kAttackTelegraphLineSphereStep = 0.24f;
inline constexpr uint32_t kAttackTelegraphLineSphereSubdivision = 3;

void DrawAttackTelegraphSegments(
    const std::vector<AttackTelegraphSegment> &segments);

} // namespace Atrum
