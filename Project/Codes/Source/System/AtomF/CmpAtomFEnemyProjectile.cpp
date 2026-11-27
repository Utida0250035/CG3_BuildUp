#include "System/AtomF/CmpAtomFEnemyProjectile.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Particle/GameCombatParticleEffects.h"

namespace Atrum {

void CmpAtomFEnemyProjectile::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  hitSphere_ = RefOwner().HasUpdCmp<CmpHitSphere>()
                   ? RefOwner().GetUpdCmp<CmpHitSphere>()
                   : nullptr;
}

void CmpAtomFEnemyProjectile::SetDirection(
    const Math::Vector3 &directionXZUnit) {
  direction_ = directionXZUnit;
  direction_.y = 0.0f;
  if (direction_.LengthSquare() > 1e-6f) {
    direction_.Normalize();
  } else {
    direction_ = {0.0f, 0.0f, 1.0f};
  }
}

void CmpAtomFEnemyProjectile::Vanish(const bool playHitEffect) {
  if (consumed_) {
    return;
  }
  consumed_ = true;
  if (playHitEffect) {
    Particle::Combat::PlayPlayerHitByEnemyBulletEffect(
        RefOwner().GetWorldPosition());
  }
  if (hitSphere_ != nullptr) {
    hitSphere_->SetIsActive(false);
  }
  RefOwner().Finalize();
  RefOwner().RequestDestroy();
}

void CmpAtomFEnemyProjectile::Update() {
  if (consumed_ || frameDeltaTime_ == nullptr) {
    return;
  }

  const float dt = frameDeltaTime_->GetDeltaTime();
  elapsedSeconds_ += dt;
  RefOwner().RefTransform().translate +=
      direction_ * (speedMetersPerSecond_ * dt);

  // 自機に触れたら消える（ダメージは CmpHealthHit 側が与える）。
  if (hitSphere_ != nullptr && hitSphere_->GetIsActive()) {
    EntityTagRegistry &tags = EntityTagRegistry::Instance();
    tags.EnsureLoaded();
    for (Entity *other : hitSphere_->GetHitEntitys()) {
      if (other != nullptr && tags.EntityHasRole(*other, "player")) {
        Vanish(true);
        return;
      }
    }
  }

  const Math::Vector3 position = RefOwner().GetWorldPosition();
  if (AtomF::IsWorldXZTouchingArenaWall(position.x, position.z,
                                        radiusMeters_)) {
    Vanish(false);
    return;
  }

  if (elapsedSeconds_ >= lifetimeSeconds_) {
    Vanish(false);
  }
}

} // namespace Atrum
