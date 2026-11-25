#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFEnemyPredictCapsuleTargetQuery.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <cmath>
#include <cstdint>

namespace Atrum {

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

Math::Vector3 CmpAtomFEnemyTargetedPanic::ComputePanicVisualShakeOffset(
    const float phaseSeconds) const {
  const float x = std::sin(phaseSeconds * panicParams_.wobbleSpeed) *
                  panicParams_.shakeAmplitudeHorizontalMeters;
  const float z =
      std::cos(phaseSeconds * panicParams_.wobbleSpeed * 1.27f) *
      (panicParams_.shakeAmplitudeHorizontalMeters * 0.85f);
  const float y =
      std::abs(std::sin(phaseSeconds * panicParams_.wobbleSpeed * 1.65f)) *
      panicParams_.shakeAmplitudeVerticalMeters;
  return Math::Vector3{x, y, z};
}

void CmpAtomFEnemyTargetedPanic::EmitSweatFromTopIfDue(
    const float deltaSeconds, const Math::Vector3 &visualOffset) {
  sweatEmitCooldownSeconds_ -= deltaSeconds;
  if (sweatEmitCooldownSeconds_ > 0.0f) {
    return;
  }
  sweatEmitCooldownSeconds_ = panicParams_.sweatEmitIntervalSeconds;

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
  panicParams_ = AtomF::ResolveEnemyPanicParameters();
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
  AtomF::EnemyPredictTargetInfo predictInfo{};
  const bool targeted =
      AtomF::TryResolveEnemyPredictTarget(RefOwner(), predictInfo) &&
      predictInfo.targeted;

  if (targeted) {
    if (!isPanicking_) {
      wobblePhaseSeconds_ = 0.0f;
      sweatEmitCooldownSeconds_ = 0.0f;
    }
    isPanicking_ = true;
    panicLingerSeconds_ = panicParams_.lingerAfterAimSeconds;
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

  wobblePhaseSeconds_ += dt;
  const Math::Vector3 visualOffset =
      ComputePanicVisualShakeOffset(wobblePhaseSeconds_);
  ApplyVisualShakeOffset(visualOffset);
  EmitSweatFromTopIfDue(dt, visualOffset);
}

} // namespace Atrum