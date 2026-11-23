#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 自機エイムカプセルに狙われた敵の立ち止まりと焦り震え（当たり判定位置は動かさない）。
class CmpAtomFEnemyTargetedPanic final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;
  void Finalize() override;

  bool BlocksLocomotion() const { return isPanicking_; }

private:
  void ApplyVisualShakeOffset(const Math::Vector3 &offset);
  void ResetVisualShakeOffset();
  void EmitSweatFromTopIfDue(float deltaSeconds,
                             const Math::Vector3 &visualOffset);

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  bool isPanicking_ = false;
  float panicLingerSeconds_ = 0.0f;
  float wobblePhaseSeconds_ = 0.0f;
  float sweatEmitCooldownSeconds_ = 0.0f;
};

} // namespace Atrum