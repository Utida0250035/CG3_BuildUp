#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"

#include "System/AtomF/AtomFEnemyAimCapsuleTargetQuery.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kPanicLingerAfterAimSeconds = 0.35f;
constexpr float kPanicShakeAmplitudeMeters = 0.08f;
constexpr float kPanicWobbleSpeed = 62.0f;

} // namespace

void CmpAtomFEnemyTargetedPanic::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFEnemyTargetedPanic::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  if (CmpAtomFEnemyKnockback *knockback =
          RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
      knockback != nullptr && knockback->HasActiveKnockback()) {
    isPanicking_ = false;
    panicLingerSeconds_ = 0.0f;
    return;
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
      anchorTranslate_ = RefOwner().RefTransform().translate;
    }
    isPanicking_ = true;
    panicLingerSeconds_ = kPanicLingerAfterAimSeconds;
  } else if (isPanicking_) {
    panicLingerSeconds_ -= dt;
    if (panicLingerSeconds_ <= 0.0f) {
      isPanicking_ = false;
      panicLingerSeconds_ = 0.0f;
      return;
    }
  } else {
    return;
  }

  wobblePhaseSeconds_ += dt;
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate = anchorTranslate_;
  const float wobble = std::sin(wobblePhaseSeconds_ * kPanicWobbleSpeed);
  translate.x += wobble * kPanicShakeAmplitudeMeters;
  translate.z +=
      std::cos(wobblePhaseSeconds_ * kPanicWobbleSpeed * 1.2f) *
      kPanicShakeAmplitudeMeters * 0.85f;
}

} // namespace Atrum