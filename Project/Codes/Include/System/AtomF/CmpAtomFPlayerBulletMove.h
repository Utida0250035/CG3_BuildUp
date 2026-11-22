#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 自機弾の直進（速さb）。方向は発射時に固定。
class CmpAtomFPlayerBulletMove final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void SetDirection(const Math::Vector3 &directionXZUnit);
  Math::Vector3 GetDirectionXZUnit() const { return direction_; }

  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Math::Vector3 direction_{0.0f, 0.0f, 1.0f};
};

} // namespace Atrum