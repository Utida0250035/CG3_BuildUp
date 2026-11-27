#include "System/AtomF/CmpAtomFEnemyShooter.h"

#include "Math/Vector4.h"
#include "System/AtomF/AtomFEnemyProjectileSpawn.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

/// 発射前に狙いを定める時間（秒）。後半は向きを固定して避ける猶予にする。
constexpr float kAimSeconds = 0.8f;
constexpr float kAimLockRatio = 0.65f;

} // namespace

void CmpAtomFEnemyShooter::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  baseRadius_ = AtomF::ResolveEnemyBodyRadiusMeters();
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyVariant>()) {
    if (const CmpAtomFEnemyVariant *variant =
            RefOwner().GetUpdCmp<CmpAtomFEnemyVariant>()) {
      const AtomF::EnemyVariantParameters &v = variant->Parameters();
      shootIntervalSeconds_ = v.shootIntervalSeconds;
      baseColorR_ = v.colorR;
      baseColorG_ = v.colorG;
      baseColorB_ = v.colorB;
    }
  }
  // 出現直後にすぐ撃たないよう、初回は間隔の半分から数える。
  waitTimerSeconds_ = shootIntervalSeconds_ * 0.5f;
}

float CmpAtomFEnemyShooter::AimProgress() const {
  if (phase_ != Phase::Aiming) {
    return 0.0f;
  }
  return std::clamp(aimElapsedSeconds_ / kAimSeconds, 0.0f, 1.0f);
}

void CmpAtomFEnemyShooter::ApplyAimVisual(const bool aiming) {
  if (!RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    return;
  }
  CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>();
  if (mesh == nullptr) {
    return;
  }
  if (!aiming) {
    mesh->SetColor(Math::Vector4{baseColorR_, baseColorG_, baseColorB_, 1.0f});
    mesh->SetRadius(baseRadius_);
    return;
  }
  const float progress = AimProgress();
  const float pulse = 0.5f + 0.5f * std::sin(aimElapsedSeconds_ * 30.0f);
  const float k = progress * (0.4f + 0.6f * pulse);
  mesh->SetColor(Math::Vector4{baseColorR_ + (1.0f - baseColorR_) * k,
                               baseColorG_ + (1.0f - baseColorG_) * k,
                               baseColorB_ + (1.0f - baseColorB_) * k, 1.0f});
  mesh->SetRadius(baseRadius_ * (1.0f + 0.25f * progress * pulse));
}

void CmpAtomFEnemyShooter::FireProjectile() {
  Math::Vector3 origin = RefOwner().GetWorldPosition();
  Particle::Combat::PlayEnemyMuzzleFlashEffect(origin);
  AtomF::SpawnEnemyProjectile(origin + aimDirection_ * (baseRadius_ + 0.1f),
                              aimDirection_, baseColorR_, baseColorG_,
                              baseColorB_);
}

void CmpAtomFEnemyShooter::Update() {
  if (frameDeltaTime_ == nullptr || !IsEnabled()) {
    return;
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      return;
    }
  }

  // 吹き飛び中・焦り中は予備動作を中断してやり直す。
  bool interrupted = false;
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    if (CmpAtomFEnemyKnockback *knockback =
            RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
        knockback != nullptr && knockback->HasActiveKnockback()) {
      interrupted = true;
    }
  }
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyTargetedPanic>()) {
    if (CmpAtomFEnemyTargetedPanic *panic =
            RefOwner().GetUpdCmp<CmpAtomFEnemyTargetedPanic>();
        panic != nullptr && panic->BlocksLocomotion()) {
      interrupted = true;
    }
  }
  if (interrupted) {
    if (phase_ == Phase::Aiming) {
      phase_ = Phase::Waiting;
      waitTimerSeconds_ = shootIntervalSeconds_ * 0.5f;
      ApplyAimVisual(false);
    }
    return;
  }

  const float dt = frameDeltaTime_->GetDeltaTime();
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  const bool hasPlayer = player != nullptr && player->IsActive();

  if (phase_ == Phase::Waiting) {
    waitTimerSeconds_ += dt;
    if (hasPlayer && waitTimerSeconds_ >= shootIntervalSeconds_) {
      phase_ = Phase::Aiming;
      aimElapsedSeconds_ = 0.0f;
    }
    return;
  }

  aimElapsedSeconds_ += dt;
  if (hasPlayer && AimProgress() < kAimLockRatio) {
    Math::Vector3 dir = player->GetWorldPosition() - RefOwner().GetWorldPosition();
    dir.y = 0.0f;
    if (dir.LengthSquare() > 1e-6f) {
      dir.Normalize();
      aimDirection_ = dir;
    }
  }
  ApplyAimVisual(true);

  if (aimElapsedSeconds_ >= kAimSeconds) {
    FireProjectile();
    phase_ = Phase::Waiting;
    waitTimerSeconds_ = 0.0f;
    ApplyAimVisual(false);
  }
}

} // namespace Atrum
