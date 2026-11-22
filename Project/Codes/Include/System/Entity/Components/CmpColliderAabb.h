#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentData.h"

namespace Atrum {

/// ローカル空間 AABB（ランタイム物理・ヒットボックス用 Data。挙動は別コンポーネント）
class CmpColliderAabb : public ComponentData {
private:
  Math::Vector3 localCenter_{};
  Math::Vector3 halfExtents_{0.5f, 0.5f, 0.5f};
  bool isTrigger_ = false;

public:
  const Math::Vector3 &LocalCenter() const { return localCenter_; }
  void SetLocalCenter(const Math::Vector3 &center) { localCenter_ = center; }

  const Math::Vector3 &HalfExtents() const { return halfExtents_; }
  void SetHalfExtents(const Math::Vector3 &extents) {
    halfExtents_ = extents;
  }

  bool IsTrigger() const { return isTrigger_; }
  void SetTrigger(const bool trigger) { isTrigger_ = trigger; }
};

} // namespace Atrum