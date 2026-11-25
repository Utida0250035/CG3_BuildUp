#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFPlayerBulletMove;

/// atomF 自機弾の進行方向へ伸びる予測カプセル（弾と同じ半径・壁／敵で終端）。
class CmpAtomFPlayerBulletPredictCapsule final : public ComponentUpd {
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
  Math::Vector3 DirectionXZUnit() const;

private:
  CmpAtomFPlayerBulletMove *move_ = nullptr;
  float currentLengthMeters_ = 0.0f;
  float radiusMeters_ = 0.25f;
};

} // namespace Atrum
