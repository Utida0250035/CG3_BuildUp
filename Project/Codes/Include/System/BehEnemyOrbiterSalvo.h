#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

/// 各オービター位置から自機狙いの弾を順番に1発ずつ（連射しない）
class BehEnemyOrbiterSalvo : public Behavior {
private:
  enum class Phase { VolleyTelegraph, OrbiterTelegraph, IntervalWait };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::VolleyTelegraph;
  float timer_ = 0.0f;
  uint32_t volleyIndex_ = 0;
  uint32_t volleyCountMax_ = 2;
  size_t orbiterFireIndex_ = 0;
  size_t orbiterCount_ = 0;
  bool warningPending_ = false;

  static constexpr float kVolleyTelegraphSeconds = 0.55f;
  static constexpr float kPerOrbiterTelegraphSeconds = 0.45f;
  static constexpr float kIntervalBetweenVolleysSeconds = 0.85f;
  static constexpr float kBulletSpeed = 8.0f;

  void BeginVolleyTelegraph();
  void FireFromOrbiterIndex(size_t index);
  void TransitionToRushIfFinished();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
