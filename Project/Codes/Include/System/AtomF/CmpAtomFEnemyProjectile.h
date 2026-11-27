#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 敵・ボスが撃つ直進弾。自機に当たる／壁に当たる／寿命切れで消える。
/// ダメージは ENEMY_WEAPON タグ＋CmpHitSphere を自機の CmpHealthHit が拾って与える。
class CmpAtomFEnemyProjectile final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_RESPONSE;
  }

  void ResolveDependence() override;
  void Update() override;

  void SetVelocity(const Math::Vector3 &velocityXZ) { velocity_ = velocityXZ; }
  void SetRadiusMeters(const float radius) { radiusMeters_ = radius; }
  void SetLifetimeSeconds(const float seconds) { lifetimeSeconds_ = seconds; }

private:
  void Vanish(bool playImpactEffect);

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Math::Vector3 velocity_{};
  float radiusMeters_ = 0.14f;
  float lifetimeSeconds_ = 8.0f;
  float ageSeconds_ = 0.0f;
  bool vanished_ = false;
};

} // namespace Atrum
