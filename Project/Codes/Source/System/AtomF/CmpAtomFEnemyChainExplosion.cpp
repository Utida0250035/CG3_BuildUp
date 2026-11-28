#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"

#include <algorithm>

#include "System/AtomF/AtomFEnemyArenaWallContact.h"
#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"
#include "System/AtomF/AtomFEnemyExplosionPlayerDamage.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "Math/Vector4.h"
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

void CmpAtomFEnemyChainExplosion::BeginDyingSilently() {
  if (isDying_) {
    return;
  }
  isDying_ = true;
  shrinkElapsedSeconds_ = 0.0f;
  if (RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    if (const CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
      initialMeshColor_ = mesh->GetColor();
    }
  }
  Particle::Combat::PlayNamedOneShotEffect("atomf_enemy_explosion.json",
                                           RefOwner().GetWorldPosition());
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

void CmpAtomFEnemyChainExplosion::BeginDyingExplosion() {
  if (isDying_) {
    return;
  }
  isDying_ = true;
  shrinkElapsedSeconds_ = 0.0f;
  if (RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    if (const CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
      initialMeshColor_ = mesh->GetColor();
    }
  }

  const Math::Vector3 explosionCenter = RefOwner().GetWorldPosition();
  Particle::Combat::PlayNamedOneShotEffect("atomf_enemy_explosion.json",
                                           explosionCenter);

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
      Math::Vector4 color = initialMeshColor_;
      color.w = scale;
      mesh->SetColor(color);
    }
  }

  UpdateDeathRings(t);

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
namespace Atrum {

/// 種類ごとに形の違う床の輪で爆散を表す（色は種類色）。
/// 射手系: 細い輪を 3 重の波紋に / 周回系: 外へ広がる輪と内へ縮む輪 /
/// 高速系: 大きく素早い 1 重の輪 / それ以外: 種類色の輪と白い核の輪。
void CmpAtomFEnemyChainExplosion::UpdateDeathRings(const float t) {
  if (!RefOwner().HasDrawCmp<CmpDrawAtomFTelegraph>()) {
    return;
  }
  CmpDrawAtomFTelegraph *telegraph =
      RefOwner().GetDrawCmp<CmpDrawAtomFTelegraph>();
  if (telegraph == nullptr) {
    return;
  }
  telegraph->Clear();
  if (t >= 1.0f) {
    return;
  }

  AtomF::EnemyVariantParameters vp{};
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyVariant>()) {
    if (const CmpAtomFEnemyVariant *variant =
            RefOwner().GetUpdCmp<CmpAtomFEnemyVariant>()) {
      vp = variant->Parameters();
    }
  }
  Math::Vector3 center = RefOwner().GetWorldPosition();
  center.y = 0.0f;
  const float fade = 1.0f - t;
  const float base = initialMeshRadius_;
  const Math::Vector4 tint{vp.colorR, vp.colorG, vp.colorB, 0.85f * fade};

  if (vp.shotIntervalSeconds > 0.0f) {
    for (int i = 0; i < 3; ++i) {
      const float delay = 0.18f * static_cast<float>(i);
      const float tt = std::clamp((t - delay) / (1.0f - delay), 0.0f, 1.0f);
      if (tt <= 0.0f || tt >= 1.0f) {
        continue;
      }
      telegraph->AddRing(center, base * (1.0f + 4.0f * tt), 0.03f,
                         Math::Vector4{vp.colorR, vp.colorG, vp.colorB,
                                       0.8f * (1.0f - tt)});
    }
  } else if (vp.orbitRadiusMeters > 0.0f) {
    telegraph->AddRing(center, base * (1.0f + 4.5f * t), 0.06f, tint);
    telegraph->AddRing(center, base * (3.0f - 2.5f * t), 0.04f,
                       Math::Vector4{1.0f, 1.0f, 1.0f, 0.7f * fade});
  } else if (vp.moveSpeedMultiplier >= 1.5f) {
    telegraph->AddRing(center, base * (1.0f + 7.0f * t), 0.08f, tint);
  } else {
    telegraph->AddRing(center, base * (1.0f + 3.5f * t), 0.06f, tint);
    telegraph->AddRing(center, base * (1.0f + 1.5f * t), 0.04f,
                       Math::Vector4{1.0f, 1.0f, 1.0f, 0.7f * fade});
  }
}

} // namespace Atrum
