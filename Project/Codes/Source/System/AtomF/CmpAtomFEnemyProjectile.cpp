#include "System/AtomF/CmpAtomFEnemyProjectile.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

namespace Atrum {

void CmpAtomFEnemyProjectile::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFEnemyProjectile::Vanish(const bool playImpactEffect) {
  if (vanished_) {
    return;
  }
  vanished_ = true;
  if (playImpactEffect) {
    Particle::Combat::PlayPlayerHitByEnemyBulletEffect(
        RefOwner().GetWorldPosition());
  }
  if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
      hit->SetIsActive(false);
    }
  }
  RefOwner().Finalize();
  RefOwner().RequestDestroy();
}

void CmpAtomFEnemyProjectile::Update() {
  if (vanished_ || frameDeltaTime_ == nullptr) {
    return;
  }
  const float dt = frameDeltaTime_->GetDeltaTime();
  ageSeconds_ += dt;
  RefOwner().RefTransform().translate += velocity_ * dt;

  const Math::Vector3 position = RefOwner().GetWorldPosition();
  if (ageSeconds_ >= lifetimeSeconds_) {
    Vanish(false);
    return;
  }
  if (AtomF::IsWorldXZTouchingArenaWall(position.x, position.z,
                                        radiusMeters_)) {
    Vanish(true);
    return;
  }

  // 自機に触れたら消える（ダメージ判定は自機側の CmpHealthHit が先に拾う）。
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player != nullptr && player->IsActive() &&
      RefOwner().HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
      for (const Entity *other : hit->GetHitEntitys()) {
        if (other == player) {
          Vanish(false);
          return;
        }
      }
    }
  }
}

} // namespace Atrum
