#pragma once

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 敵の parameters 間隔ごとに震えてから自機へ突進する。
class CmpAtomFEnemyPeriodicCharge final : public ComponentUpd {
public:
  enum class Phase { Waiting, Shaking, Charging };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  bool BlocksLocomotion() const {
    return phase_ == Phase::Shaking || phase_ == Phase::Charging;
  }

  Phase CurrentPhase() const { return phase_; }

private:
  void BeginShaking();
  void BeginCharging();

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Phase phase_ = Phase::Waiting;
  float waitTimerSeconds_ = 0.0f;
  float phaseElapsedSeconds_ = 0.0f;
  float chargeIntervalSeconds_ = 5.0f;
  AtomF::EnemyPeriodicChargeTimingParameters chargeTiming_{};
  Math::Vector3 anchorTranslate_{};
};

} // namespace Atrum