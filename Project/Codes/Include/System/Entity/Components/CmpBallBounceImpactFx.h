#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"
#include <string>

namespace Atrum {

/// バウンス衝突点のワンショットパーティクル（運動・反発とは分離）
class CmpBallBounceImpactFx : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PHYSICS;
  }

  void Initialize() override;
  void Update() override;

  void SetImpactJsonPath(const std::string &path) { impactJsonPath_ = path; }
  void SetImpactCooldownSeconds(const float seconds) {
    impactCooldownSeconds_ = seconds;
  }

  /// クールダウンと速度条件を満たすときだけ演出を出す
  void TrySpawnImpactAt(const Math::Vector3 &worldPosition,
                        const float verticalSpeedMagnitude,
                        const bool enforceVerticalSpeedGate);

private:
  void SpawnImpactAt(const Math::Vector3 &worldPosition);

  std::string impactJsonPath_{"demos/ball_wall_shockwave.json"};
  float impactCooldownSeconds_ = 0.12f;

  float impactCooldownRemaining_ = 0.0f;
  uint32_t impactSequence_ = 0;
};

} // namespace Atrum