#include "System/AtomF/CmpAtomFPlayerShotCooldown.h"

#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/DeltaTime.h"

namespace Atrum {

namespace {

constexpr float kDefaultShotCooldownSeconds = 1.0f;

} // namespace

void CmpAtomFPlayerShotCooldown::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

float CmpAtomFPlayerShotCooldown::GetShotCooldownSeconds() const {
  const AtomF::ParametersRuntime &runtime = AtomF::ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().shotCooldownSeconds;
  }
  return kDefaultShotCooldownSeconds;
}

void CmpAtomFPlayerShotCooldown::Update() {
  if (frameDeltaTime_ == nullptr || cooldownRemainingSeconds_ <= 0.0f) {
    return;
  }

  cooldownRemainingSeconds_ -= frameDeltaTime_->GetDeltaTime();
  if (cooldownRemainingSeconds_ < 0.0f) {
    cooldownRemainingSeconds_ = 0.0f;
  }
}

bool CmpAtomFPlayerShotCooldown::TryConsumeShotCooldown() {
  if (!IsShotReady()) {
    return false;
  }

  cooldownRemainingSeconds_ = GetShotCooldownSeconds();
  return true;
}

} // namespace Atrum