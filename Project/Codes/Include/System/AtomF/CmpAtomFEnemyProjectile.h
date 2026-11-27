#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpHitSphere;

/// atomF 敵（雑魚の射撃型・ボス）が撃つ弾。直進し、壁・自機・寿命切れで消える。
/// 自機へのダメージは ENEMY_WEAPON タグ + CmpHitSphere を CmpHealthHit が拾って与える。
class CmpAtomFEnemyProjectile final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  void SetDirection(const Math::Vector3 &directionXZUnit);
  void SetSpeedMetersPerSecond(const float speed) { speedMetersPerSecond_ = speed; }
  void SetRadiusMeters(const float radius) { radiusMeters_ = radius; }
  void SetLifetimeSeconds(const float seconds) { lifetimeSeconds_ = seconds; }

private:
  void Vanish(const bool playHitEffect);

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpHitSphere *hitSphere_ = nullptr;
  Math::Vector3 direction_{0.0f, 0.0f, 1.0f};
  float speedMetersPerSecond_ = 3.0f;
  float radiusMeters_ = 0.18f;
  float lifetimeSeconds_ = 6.0f;
  float elapsedSeconds_ = 0.0f;
  bool consumed_ = false;
};

} // namespace Atrum
