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
  void ApplyKnockbackAwayFromExplosionCenter(
      const Math::Vector3 &explosionCenterWorld,
      const float speedMetersPerSecond);

  bool HasActiveKnockback() const {
    return velocity_.LengthSquare() > 1e-8f;
  }

  /// 現在の吹き飛び速度（ワールド XZ）。演出の向き決めに使う。
  const Math::Vector3 &GetVelocity() const { return velocity_; }

  /// 自機弾や敵爆発など、連鎖爆発の契機になったノックバック中。
  bool IsKnockbackFromPlayerWeapon() const {
    return chainImpactKnockback_;
  }

  void ClearKnockbackVelocity();

  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Math::Vector3 velocity_{};
  bool chainImpactKnockback_ = false;
};

} // namespace Atrum