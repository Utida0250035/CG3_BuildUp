#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFEnemyAimCapsuleTargetQuery.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <cmath>
#include <cstdint>

namespace Atrum {

namespace {

constexpr float kPanicLingerAfterAimSeconds = 0.35f;
// 斜め俯瞰カメラ（距離約14m）からでも敵半径0.25mに対してぱっと見で分かる振幅・周波数。
constexpr float kPanicShakeAmplitudeHorizontalMeters = 0.12f;
constexpr float kPanicShakeAmplitudeVerticalMeters = 0.045f;
constexpr float kPanicWobbleSpeed = 76.0f;
constexpr float kPanicSweatEmitIntervalSeconds = 0.14f;

/// 焦り中の見た目専用小刻み震えオフセットを算出する（地面に沈まないよう Y >= 0）。
Math::Vector3 ComputePanicVisualShakeOffset(const float phaseSeconds) {
  const float x =
      std::sin(phaseSeconds * kPanicWobbleSpeed) *
      kPanicShakeAmplitudeHorizontalMeters;
  const float z =
      std::cos(phaseSeconds * kPanicWobbleSpeed * 1.27f) *
      (kPanicShakeAmplitudeHorizontalMeters * 0.85f);
  const float y =
      std::abs(std::sin(phaseSeconds * kPanicWobbleSpeed * 1.65f)) *
      kPanicShakeAmplitudeVerticalMeters;
  return Math::Vector3{x, y, z};
}

} // namespace

void CmpAtomFEnemyTargetedPanic::ApplyVisualShakeOffset(
    const Math::Vector3 &offset) {
  if (GetOwner() == nullptr) {
    return;
  }
  Entity &owner = RefOwner();
  if (!owner.HasDrawCmp<CmpMeshSphere>()) {
    return;
  }
  if (CmpMeshSphere *mesh = owner.GetDrawCmp<CmpMeshSphere>();
      mesh != nullptr) {
    mesh->SetDrawOffset(offset);
  }
}

void CmpAtomFEnemyTargetedPanic::ResetVisualShakeOffset() {
  ApplyVisualShakeOffset(Math::Vector3{0.0f, 0.0f, 0.0f});
}

void CmpAtomFEnemyTargetedPanic::EmitSweatFromTopIfDue(
    const float deltaSeconds, const Math::Vector3 &visualOffset) {
  sweatEmitCooldownSeconds_ -= deltaSeconds;
  if (sweatEmitCooldownSeconds_ > 0.0f) {
    return;
  }
  sweatEmitCooldownSeconds_ = kPanicSweatEmitIntervalSeconds;

  const float topOffsetY = AtomF::ResolveEnemyBodyRadiusMeters();
  const Math::Vector3 topWorldPos =
      RefOwner().GetWorldPosition() + visualOffset +
      Math::Vector3{0.0f, topOffsetY, 0.0f};
  static uint32_t sweatSequence = 0U;
  ++sweatSequence;
  Particle::Combat::PlayEnemyPanicSweatEffect(topWorldPos, sweatSequence);
}

void CmpAtomFEnemyTargetedPanic::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFEnemyTargetedPanic::Finalize() {
  isPanicking_ = false;
  panicLingerSeconds_ = 0.0f;
  sweatEmitCooldownSeconds_ = 0.0f;
  ResetVisualShakeOffset();
}

void CmpAtomFEnemyTargetedPanic::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      isPanicking_ = false;
      panicLingerSeconds_ = 0.0f;
      sweatEmitCooldownSeconds_ = 0.0f;
      ResetVisualShakeOffset();
      return;
    }
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    if (CmpAtomFEnemyKnockback *knockback =
            RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
        knockback != nullptr && knockback->HasActiveKnockback()) {
      isPanicking_ = false;
      panicLingerSeconds_ = 0.0f;
      sweatEmitCooldownSeconds_ = 0.0f;
      ResetVisualShakeOffset();
      return;
    }
  }

  const float dt = frameDeltaTime_->GetDeltaTime();
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  bool targeted = false;
  if (player != nullptr && player->IsActive()) {
    targeted =
        AtomF::IsEnemyTargetedByPlayerAimCapsule(RefOwner(), *player);
  }

  if (targeted) {
    if (!isPanicking_) {
      wobblePhaseSeconds_ = 0.0f;
      sweatEmitCooldownSeconds_ = 0.0f;
    }
    isPanicking_ = true;
    panicLingerSeconds_ = kPanicLingerAfterAimSeconds;
  } else if (isPanicking_) {
    panicLingerSeconds_ -= dt;
    if (panicLingerSeconds_ <= 0.0f) {
      isPanicking_ = false;
      panicLingerSeconds_ = 0.0f;
      sweatEmitCooldownSeconds_ = 0.0f;
      ResetVisualShakeOffset();
      return;
    }
  } else {
    return;
  }

  // 当たり判定（Entity の Transform / CmpHitSphere）は動かさず、見た目だけ小刻みに震わせる。
  wobblePhaseSeconds_ += dt;
  const Math::Vector3 visualOffset =
      ComputePanicVisualShakeOffset(wobblePhaseSeconds_);
  ApplyVisualShakeOffset(visualOffset);
  EmitSweatFromTopIfDue(dt, visualOffset);
}

} // namespace Atrum