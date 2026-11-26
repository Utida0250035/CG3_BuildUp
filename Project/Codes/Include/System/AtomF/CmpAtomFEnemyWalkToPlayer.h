#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 敵が自機へ parameters 速さa で XZ 歩行する。
class CmpAtomFEnemyWalkToPlayer final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  float weaveTimeSeconds_ = 0.0f;
};

} // namespace Atrum