#include "System/AtomF/CmpAtomFPlayerAimCapsule.h"

#include "System/AtomF/CmpAtomFPlayerAim.h"
#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFPlayerAimCapsuleRayClamp.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

namespace {

constexpr float kStretchMetersPerSecond = 10.0f;
constexpr float kShrinkMetersPerSecond = 14.0f;

float MaxAimCapsuleLengthMeters() {
  return AtomF::kArenaInnerRadiusMeters * 2.0f;
}

} // namespace

void CmpAtomFPlayerAimCapsule::ResolveDependence() {
  aim_ = RefOwner().GetUpdCmp<CmpAtomFPlayerAim>();
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  radiusMeters_ = AtomF::ResolvePlayerAimCapsuleRadiusMeters();
}

Math::Vector3 CmpAtomFPlayerAimCapsule::CapsuleStartWorld() {
  return RefOwner().GetWorldPosition();
}

Math::Vector3 CmpAtomFPlayerAimCapsule::CapsuleEndWorld() {
  const Math::Vector3 start = CapsuleStartWorld();
  if (aim_ == nullptr || currentLengthMeters_ <= 0.0f) {
    return start;
  }
  return start + aim_->GetAimDirectionXZ() * currentLengthMeters_;
}

void CmpAtomFPlayerAimCapsule::Update() {
  if (frameDeltaTime_ == nullptr || aim_ == nullptr) {
    return;
  }

  const float dt = frameDeltaTime_->GetDeltaTime();
  const float maxLength = MaxAimCapsuleLengthMeters();

  const Math::Vector3 start = CapsuleStartWorld();
  const Math::Vector3 dir = aim_->GetAimDirectionXZ();
  const float clampedMax = AtomF::ResolveAimCapsuleClampedMaxLength(
      RefOwner(), start, dir, radiusMeters_, maxLength);

  if (aim_->IsAimInputActive()) {
    currentLengthMeters_ += kStretchMetersPerSecond * dt;
    if (currentLengthMeters_ > clampedMax) {
      currentLengthMeters_ = clampedMax;
    }
    return;
  }

  if (currentLengthMeters_ <= 0.0f) {
    currentLengthMeters_ = 0.0f;
    return;
  }

  currentLengthMeters_ -= kShrinkMetersPerSecond * dt;
  if (currentLengthMeters_ < 0.0f) {
    currentLengthMeters_ = 0.0f;
  }
  if (currentLengthMeters_ > clampedMax) {
    currentLengthMeters_ = clampedMax;
  }
}

} // namespace Atrum