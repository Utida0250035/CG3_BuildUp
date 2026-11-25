#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 予測判定に当たった敵から、吹き飛び予測方向へ伸びるカプセル（敵半径）。
class CmpAtomFEnemyKnockbackPredictCapsule final : public ComponentUpd {
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
  float currentLengthMeters_ = 0.0f;
  float radiusMeters_ = 0.25f;
  Math::Vector3 directionXZ_{0.0f, 0.0f, 1.0f};
};

} // namespace Atrum
