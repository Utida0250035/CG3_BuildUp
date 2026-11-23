#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFEnemyAimCapsuleTargetQuery.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kPanicLingerAfterAimSeconds = 0.35f;
// 斜め俯瞰カメラ（距離約14m）からでも敵半径0.25mに対してぱっと見で分かる振幅・周波数。
constexpr float kPanicShakeAmplitudeHorizontalMeters = 0.12f;
constexpr float kPanicShakeAmplitudeVerticalMeters = 0.045f;
constexpr float kPanicWobbleSpeed = 76.0f;

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

void CmpAtomFEnemyTargetedPanic::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFEnemyTargetedPanic::Finalize() {
  isPanicking_ = false;
  panicLingerSeconds_ = 0.0f;
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
    }
    isPanicking_ = true;
    panicLingerSeconds_ = kPanicLingerAfterAimSeconds;
  } else if (isPanicking_) {
    panicLingerSeconds_ -= dt;
    if (panicLingerSeconds_ <= 0.0f) {
      isPanicking_ = false;
      panicLingerSeconds_ = 0.0f;
      ResetVisualShakeOffset();
      return;
    }
  } else {
    return;
  }

  // 当たり判定（Entity の Transform / CmpHitSphere）は動かさず、見た目だけ小刻みに震わせる。
  wobblePhaseSeconds_ += dt;
  ApplyVisualShakeOffset(ComputePanicVisualShakeOffset(wobblePhaseSeconds_));
}

} // namespace Atrum