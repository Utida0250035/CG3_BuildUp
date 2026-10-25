#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

/// 各オービターからボス中心の外向きに一斉射出（Volley 間のみ待機）
class BehEnemyOrbiterSpoke : public Behavior {
private:
  enum class Phase { Telegraph, IntervalWait };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::Telegraph;
  float timer_ = 0.0f;
  uint32_t volleyIndex_ = 0;
  uint32_t volleyCountMax_ = 2;
  bool warningPending_ = false;

  static constexpr float kTelegraphSeconds = 0.7f;
  static constexpr float kIntervalBetweenVolleysSeconds = 0.9f;
  static constexpr float kBulletSpeed = 7.0f;

  void FireSpokeVolley();
  void BeginTelegraph();
  void TransitionToRushIfFinished();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
