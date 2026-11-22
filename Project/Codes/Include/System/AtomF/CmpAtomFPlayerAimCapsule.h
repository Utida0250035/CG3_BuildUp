#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFPlayerAim;

/// atomF 自機エイム中に XZ 上へ伸びるカプセル（長さのみ。半径・衝突終端は別タスク）。
class CmpAtomFPlayerAimCapsule final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  float CurrentLengthMeters() const { return currentLengthMeters_; }
  Math::Vector3 CapsuleStartWorld();
  Math::Vector3 CapsuleEndWorld();

private:
  CmpAtomFPlayerAim *aim_ = nullptr;
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  float currentLengthMeters_ = 0.0f;
};

} // namespace Atrum