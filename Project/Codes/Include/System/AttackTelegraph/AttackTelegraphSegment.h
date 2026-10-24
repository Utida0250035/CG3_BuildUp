#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Atrum {

/// ボス攻撃の軌道予測線1本（将来的に警告 SE と同期しやすいようデータのみ保持）
struct AttackTelegraphSegment {
  Math::Vector3 start{};
  Math::Vector3 end{};
  Math::Vector4 color{1.0f, 0.35f, 0.25f, 0.7f};
};

} // namespace Atrum
