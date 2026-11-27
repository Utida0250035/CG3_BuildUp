#include "System/AtomF/CmpAtomFEnemyShooter.h"

#include "Math/Vector4.h"
#include "System/AtomF/AtomFEnemyProjectileSpawn.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/AtomFSe.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemySpawnIn.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

/// 発射直前の照準固定（この秒数だけ狙いを更新しない）。
constexpr float kAimLockSeconds = 0.2f;

} // namespace

void CmpAtomFEnemyShooter::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyVariant>()) {
    if (const CmpAtomFEnemyVariant *variant =
            RefOwner().GetUpdCmp<CmpAtomFEnemyVariant>()) {
      const AtomF::EnemyVariantParameters &v = variant->Parameters();
      intervalSeconds_ = v.shotIntervalSeconds > 0.0f ? v.shotIntervalSeconds
                                                      : intervalSeconds_;
      telegraphSeconds_ = v.shotTelegraphSeconds;
      shotSpeed_ = v.shotSpeedMetersPerSecond;
      shotRadius_ = v.shotRadiusMeters;
      // 種類ごとの色を弾と予告線に反映する（射手/狙撃手を見分けられるように）。
      shotColor_ = Math::Vector3{v.colorR, v.colorG, v.colorB};
    }
  }
  // 最初の 1 発は出現後 intervalSeconds_ 経ってから。
  timerSeconds_ = 0.0f;
}

void CmpAtomFEnemyShooter::ResetToWaiting() {
  phase_ = Phase::Waiting;
  timerSeconds_ = 0.0f;
  if (RefOwner().HasDrawCmp<CmpDrawAtomFTelegraph>()) {
    if (CmpDrawAtomFTelegraph *telegraph =
            RefOwner().GetDrawCmp<CmpDrawAtomFTelegraph>()) {
      telegraph->Clear();
    }
  }
  if (RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    if (CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
      mesh->SetDrawOffset(Math::Vector3{});
    }
  }
}

void CmpAtomFEnemyShooter::Fire() {
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  const Math::Vector3 muzzle = origin + lockedDirection_ * 0.35f;
  AtomF::SpawnEnemyProjectile(muzzle, lockedDirection_, shotSpeed_,
                              shotRadius_,
                              Math::Vector4{shotColor_.x, shotColor_.y,
                                            shotColor_.z, 1.0f});
  Particle::Combat::PlayNamedOneShotEffect("atomf_shot_muzzle.json", muzzle);
  AtomF::PlayAtomFEnemyShotSe();
}

void CmpAtomFEnemyShooter::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  // 出現中・死亡中・吹き飛び中・焦り中は撃たない（予備動作も中断する）。
  bool suppressed = false;
  if (RefOwner().HasUpdCmp<CmpAtomFEnemySpawnIn>()) {
    if (const CmpAtomFEnemySpawnIn *spawnIn =
            RefOwner().GetUpdCmp<CmpAtomFEnemySpawnIn>()) {
      suppressed = suppressed || spawnIn->BlocksLocomotion();
    }
  }
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (const CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>()) {
      suppressed = suppressed || chain->IsDying();
    }
  }
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    if (const CmpAtomFEnemyKnockback *knockback =
            RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>()) {
      suppressed = suppressed || knockback->HasActiveKnockback();
    }
  }
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyTargetedPanic>()) {
    if (const CmpAtomFEnemyTargetedPanic *panic =
            RefOwner().GetUpdCmp<CmpAtomFEnemyTargetedPanic>()) {
      suppressed = suppressed || panic->BlocksLocomotion();
    }
  }
  if (suppressed) {
    if (phase_ != Phase::Waiting) {
      ResetToWaiting();
    }
    return;
  }

  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return;
  }

  const float dt = frameDeltaTime_->GetDeltaTime();
  timerSeconds_ += dt;

  if (phase_ == Phase::Waiting) {
    if (timerSeconds_ >= intervalSeconds_) {
      phase_ = Phase::Telegraph;
      timerSeconds_ = 0.0f;
      AtomF::PlayBossBulletWarnSe();
    }
    return;
  }

  // Telegraph: 震え＋照準線。発射の直前に狙いを固定する。
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  if (timerSeconds_ < telegraphSeconds_ - kAimLockSeconds) {
    Math::Vector3 toPlayer = player->GetWorldPosition() - origin;
    toPlayer.y = 0.0f;
    const float lengthSq = toPlayer.LengthSquare();
    if (lengthSq > 1e-6f) {
      lockedDirection_ = toPlayer * (1.0f / std::sqrt(lengthSq));
    }
  }

  const float progress = std::min(1.0f, timerSeconds_ / telegraphSeconds_);
  if (RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    if (CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
      const float amp = 0.03f + 0.07f * progress;
      mesh->SetDrawOffset(Math::Vector3{
          std::sin(timerSeconds_ * 90.0f) * amp, 0.0f,
          std::cos(timerSeconds_ * 77.0f) * amp * 0.7f});
    }
  }
  if (RefOwner().HasDrawCmp<CmpDrawAtomFTelegraph>()) {
    if (CmpDrawAtomFTelegraph *telegraph =
            RefOwner().GetDrawCmp<CmpDrawAtomFTelegraph>()) {
      telegraph->Clear();
      const bool locked = timerSeconds_ >= telegraphSeconds_ - kAimLockSeconds;
      const Math::Vector4 color =
          locked ? Math::Vector4{shotColor_.x, shotColor_.y, shotColor_.z, 0.8f}
                 : Math::Vector4{shotColor_.x * 0.7f, shotColor_.y * 0.7f,
                                 shotColor_.z * 0.7f,
                                 0.3f + 0.3f * progress};
      telegraph->AddLine(origin, origin + lockedDirection_ * 7.0f,
                    shotRadius_ * (locked ? 2.0f : 1.2f), color);
    }
  }

  if (timerSeconds_ >= telegraphSeconds_) {
    Fire();
    ResetToWaiting();
  }
}

} // namespace Atrum
