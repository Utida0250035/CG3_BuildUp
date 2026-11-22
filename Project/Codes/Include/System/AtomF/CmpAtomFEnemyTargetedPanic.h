#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 自機エイムカプセルに狙われた敵の立ち止まりと焦り震え。
class CmpAtomFEnemyTargetedPanic final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  bool BlocksLocomotion() const { return isPanicking_; }

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  bool isPanicking_ = false;
  float panicLingerSeconds_ = 0.0f;
  float wobblePhaseSeconds_ = 0.0f;
  Math::Vector3 anchorTranslate_{};
};

} // namespace Atrum