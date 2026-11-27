#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 雑魚敵の出現演出: 床に魔法陣（輪）が広がり、その上で球が膨らんで実体化する。
/// 出現中は当たり判定と行動が止まる（BlocksLocomotion）。
class CmpAtomFEnemySpawnIn final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;

  bool BlocksLocomotion() const { return !finished_; }
  void SetDurationSeconds(const float seconds) { durationSeconds_ = seconds; }

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  float durationSeconds_ = 0.5f;
  float elapsedSeconds_ = 0.0f;
  float fullRadius_ = 0.25f;
  bool finished_ = false;
  bool effectPlayed_ = false;
};

} // namespace Atrum
