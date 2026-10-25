#pragma once

#include "System/Entity/Behavior.h"
#include "System/StageBounds.h"
#include <array>

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

class BehEnemyMeteor : public Behavior {
private:
  enum class Phase { Telegraph, RapidFire };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::Telegraph;

  /// 連射・予告の間隔（0.25秒）
  static constexpr float kIntervalSeconds = 0.25f;
  /// 連射数・予告回数（3発）
  static constexpr uint32_t kMeteorCount = 3;

  float timer_ = 0.0f;
  uint32_t telegraphCount_ = 0;
  uint32_t shotCount_ = 0;
  bool telegraphWarningPending_ = false;
  std::array<float, kMeteorCount> targetXPositions_{};

  float bulletShotY_ = kBarrageSpawnTopY;
  float bulletSpeed_ = 24.0f;

  void FireMeteor(float targetX);
  void TransitionToRush();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
