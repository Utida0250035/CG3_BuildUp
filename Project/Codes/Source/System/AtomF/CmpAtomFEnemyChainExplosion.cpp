#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"

#include "System/AtomF/AtomFEnemyArenaWallContact.h"
#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"
#include "System/AtomF/AtomFEnemyExplosionPlayerDamage.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Particle/GameCombatParticleEffects.h"

namespace Atrum {

namespace {

bool IsTouchingOtherEnemy(Entity &self) {
  if (!self.HasUpdCmp<CmpHitSphere>()) {
    return false;
  }
  CmpHitSphere *hit = self.GetUpdCmp<CmpHitSphere>();
  if (hit == nullptr || !hit->GetIsActive()) {
    return false;
  }

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  for (Entity *other : hit->GetHitEntitys()) {
    if (other == nullptr || other == &self) {
      continue;
    }
    if (tags.EntityHasRole(*other, "enemy")) {
      return true;
    }
  }
  return false;
}

} // namespace

void CmpAtomFEnemyChainExplosion::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  initialMeshRadius_ = AtomF::ResolveEnemyBodyRadiusMeters();
  shrinkDurationSeconds_ =
      AtomF::ResolveEnemyChainExplosionParameters().shrinkDurationSeconds;
}

void CmpAtomFEnemyChainExplosion::BeginDyingExplosion() {
  if (isDying_) {
    return;
  }
  isDying_ = true;
  shrinkElapsedSeconds_ = 0.0f;

  const Math::Vector3 explosionCenter = RefOwner().GetWorldPosition();
  static uint32_t explosionSequence = 0U;
  ++explosionSequence;
  Particle::Combat::PlayBossDeathExplosionEffect(explosionCenter,
                                                 explosionSequence);

  AtomF::SpreadEnemyExplosionKnockbackFromCenter(RefOwner(), explosionCenter);
  AtomF::ApplyEnemyExplosionDamageToPlayer(explosionCenter);
  AtomF::ApplyEnemyExplosionDamageToBoss(explosionCenter);

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    if (CmpAtomFEnemyKnockback *knockback =
            RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>()) {
      knockback->ClearKnockbackVelocity();
    }
  }
  if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
      hit->SetIsActive(false);
    }
  }
}

void CmpAtomFEnemyChainExplosion::UpdateDying() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  shrinkElapsedSeconds_ += frameDeltaTime_->GetDeltaTime();
  const float t =
      std::min(1.0f, shrinkElapsedSeconds_ / shrinkDurationSeconds_);
  const float scale = 1.0f - t;

  if (RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    if (CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
      mesh->SetRadius(initialMeshRadius_ * scale);
      Math::Vector4 color = Math::Vector4{0.92f, 0.28f, 0.25f, 1.0f};
      color.w = scale;
      mesh->SetColor(color);
    }
  }

  if (shrinkElapsedSeconds_ >= shrinkDurationSeconds_) {
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

void CmpAtomFEnemyChainExplosion::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  if (isDying_) {
    UpdateDying();
    return;
  }

  if (!RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    return;
  }
  CmpAtomFEnemyKnockback *knockback =
      RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
  if (knockback == nullptr || !knockback->HasActiveKnockback() ||
      !knockback->IsKnockbackFromPlayerWeapon()) {
    return;
  }

  if (AtomF::IsEnemyTouchingArenaWall(RefOwner()) ||
      IsTouchingOtherEnemy(RefOwner())) {
    BeginDyingExplosion();
  }
}

} // namespace Atrum