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
constexpr float kPanicShakeAmplitudeMeters = 0.08f;
constexpr float kPanicWobbleSpeed = 62.0f;

} // namespace

void CmpAtomFEnemyTargetedPanic::ApplyVisualShakeOffset(const float offsetX,
                                                        const float offsetZ) {
  if (GetOwner() == nullptr) {
    return;
  }
  Entity &owner = RefOwner();
  if (!owner.HasDrawCmp<CmpMeshSphere>()) {
    return;
  }
  if (CmpMeshSphere *mesh = owner.GetDrawCmp<CmpMeshSphere>();
      mesh != nullptr) {
    mesh->SetDrawOffset(Math::Vector3{offsetX, 0.0f, offsetZ});
  }
}

void CmpAtomFEnemyTargetedPanic::ResetVisualShakeOffset() {
  ApplyVisualShakeOffset(0.0f, 0.0f);
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

  // 当たり判定（Entity の Transform / CmpHitSphere）は動かさず、見た目だけ揺らす。
  wobblePhaseSeconds_ += dt;
  const float wobble = std::sin(wobblePhaseSeconds_ * kPanicWobbleSpeed);
  const float offsetX = wobble * kPanicShakeAmplitudeMeters;
  const float offsetZ =
      std::cos(wobblePhaseSeconds_ * kPanicWobbleSpeed * 1.2f) *
      kPanicShakeAmplitudeMeters * 0.85f;
  ApplyVisualShakeOffset(offsetX, offsetZ);
}

} // namespace Atrum