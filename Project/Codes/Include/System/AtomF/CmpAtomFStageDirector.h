#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF ステージ進行役。雑魚が全滅したら少し間を置いてボスを出現させる（1 回だけ）。
class CmpAtomFStageDirector final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  bool seenEnemy_ = false;
  bool bossSpawned_ = false;
  float clearedElapsedSeconds_ = 0.0f;
};

} // namespace Atrum
