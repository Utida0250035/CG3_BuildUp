#pragma once

#include "System/Entity/BehaviorBox.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class Entity;

class FrameDeltaTime;

class CmpEnemy : public ComponentUpd {
private:
  Entity *player_ = nullptr;
  BehaviorBox behaviorBox_{};
  FrameDeltaTime *frameDeltaTime_ = nullptr;

  uint64_t behaviorTransitionCountForShotMeteor_ = 10;
  uint64_t behaviorTransitionCountForWallShot_ = 6;
  uint64_t behaviorTransitionCountForOrbiterSweep_ = 8;

  /// シーン入場直後はメテオ／壁弾などの強制パターン遷移を抑止する
  float patternTransitionWarmupTimer_ = 0.0f;
  static constexpr float kPatternTransitionWarmupTime = 3.0f;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::INPUT; }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;

  BehaviorBox &RefBehaviorBox() { return behaviorBox_; }
  Entity *GetPlayer() { return player_; }
  void SetPlayer(Entity *player) { player_ = player; }
};

} // namespace Atrum
