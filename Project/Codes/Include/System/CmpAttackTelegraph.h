#pragma once

#include "System/AttackTelegraph/AttackTelegraphSegment.h"
#include "System/Entity/ComponentData.h"

#include <vector>

namespace Atrum {

/// フレーム内の攻撃予測線（BehEnemy* が登録、Draw で描画）。
/// 将来は同一タイミングで警告 SE を鳴らす想定。
class CmpAttackTelegraph : public ComponentData {
private:
  std::vector<AttackTelegraphSegment> segments_{};

public:
  void Clear() { segments_.clear(); }

  void AddSegment(const AttackTelegraphSegment &segment) {
    segments_.push_back(segment);
  }

  void AddLine(const Math::Vector3 &start, const Math::Vector3 &end,
               const Math::Vector4 &color = AttackTelegraphSegment{}.color) {
    AttackTelegraphSegment segment{};
    segment.start = start;
    segment.end = end;
    segment.color = color;
    segments_.push_back(segment);
  }

  const std::vector<AttackTelegraphSegment> &GetSegments() const {
    return segments_;
  }
};

} // namespace Atrum
