#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFPlayerAim;

/// atomF 自機エイム中に XZ 上へ伸びるカプセル。壁・敵 Hit で長さをクランプする。
class CmpAtomFPlayerAimCapsule final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  float CurrentLengthMeters() const { return currentLengthMeters_; }
  float RadiusMeters() const { return radiusMeters_; }
  Math::Vector3 CapsuleStartWorld();
  Math::Vector3 CapsuleEndWorld();

private:
  CmpAtomFPlayerAim *aim_ = nullptr;
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  float currentLengthMeters_ = 0.0f;
  float radiusMeters_ = 0.1f;
};

} // namespace Atrum