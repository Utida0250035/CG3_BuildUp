#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 射手型の雑魚: 一定間隔で予備動作（メッシュが震え、自機へ照準線が伸びる）の後、弾を撃つ。
class CmpAtomFEnemyShooter final : public ComponentUpd {
public:
  enum class Phase { Waiting, Telegraph };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  bool BlocksLocomotion() const { return phase_ == Phase::Telegraph; }

private:
  void ResetToWaiting();
  void Fire();

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Phase phase_ = Phase::Waiting;
  float timerSeconds_ = 0.0f;
  float intervalSeconds_ = 3.0f;
  float telegraphSeconds_ = 0.7f;
  float shotSpeed_ = 2.4f;
  float shotRadius_ = 0.14f;
  Math::Vector3 shotColor_{1.0f, 0.35f, 0.3f};
  Math::Vector3 lockedDirection_{0.0f, 0.0f, 1.0f};
};

} // namespace Atrum
