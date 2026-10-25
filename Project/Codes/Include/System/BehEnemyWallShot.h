#pragma once

#include "System/Entity/Behavior.h"
#include "System/StageBounds.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

class BehEnemyWallShot : public Behavior {
private:
  enum class Phase { Telegraph, IntervalWait };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::Telegraph;
  float telegraphTimer_ = 0.0f;
  float intervalBetweenVolleys_ = 0.75f;
  float intervalTimer_ = 0.0f;

  float rapidBulletSpeed_ = 12.0f;
  float slowBulletSpeed_ = 5.0f;
  float bulletShotY_ = kBarrageSpawnTopY;
  uint32_t shotCount_ = 0;
  uint32_t shotCountMax_ = 2;
  bool telegraphWarningPending_ = false;

  void FireVolley();
  void BeginTelegraphPhase();
  void TransitionToRushIfFinished();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
