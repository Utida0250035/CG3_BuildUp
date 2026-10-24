#pragma once

#include "System/Entity/Behavior.h"
#include "System/StageBounds.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

class BehEnemyMeteor : public Behavior {
private:
  enum class Phase { Telegraph, IntervalWait };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::Telegraph;
  float telegraphTimer_ = 0.0f;
  /// 1 発降ろしたあと、次の予測線までの間隔
  float intervalAfterShot_ = 0.25f;
  float intervalTimer_ = 0.0f;

  float bulletShotY_ = kBarrageSpawnTopY;
  float bulletSpeed_ = 24.0f;
  uint32_t shotCount_ = 0;
  uint32_t shotCountMax_ = 15;

  void FireMeteor();
  void BeginTelegraphPhase();
  void TransitionToRushIfFinished();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
