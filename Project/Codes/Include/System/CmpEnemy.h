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

  /// シーン入場直後はメテオ／壁弾などの強制パターン遷移を抑止する
  float patternTransitionWarmupTimer_ = 0.0f;
  static constexpr float kPatternTransitionWarmupTime = 3.0f;

  uint32_t shotPatternIndex_ = 0;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::INPUT; }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;

  /// 行動ループで次の射撃挙動（通常・拡散・連射）へ遷移する
  void TransitionNextShotBehavior();

  BehaviorBox &RefBehaviorBox() { return behaviorBox_; }
  Entity *GetPlayer() { return player_; }
  void SetPlayer(Entity *player) { player_ = player; }
};

} // namespace Atrum
