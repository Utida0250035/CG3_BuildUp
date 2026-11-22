#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 敵の吹き飛び（XZ 速度を毎フレーム位置に足す）。
class CmpAtomFEnemyKnockback final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void ApplyKnockbackAlongBulletDirection(const Math::Vector3 &directionXZUnit,
                                          const float speedMetersPerSecond);

  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Math::Vector3 velocity_{};
};

} // namespace Atrum