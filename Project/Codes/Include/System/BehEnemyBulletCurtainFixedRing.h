#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

/// ボス位置から全周環状の弾幕を一斉発射（Volley 間のみ待機）
class BehEnemyBulletCurtainFixedRing : public Behavior {
private:
  enum class Phase { Telegraph, IntervalWait };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::Telegraph;
  float timer_ = 0.0f;
  uint32_t volleyIndex_ = 0;
  uint32_t volleyCountMax_ = 2;
  float ringBaseAngleRadians_ = 0.0f;
  bool warningPending_ = false;

  static constexpr float kTelegraphSeconds = 0.8f;
  static constexpr float kIntervalBetweenVolleysSeconds = 1.05f;
  static constexpr float kBulletSpeed = 5.5f;
  static constexpr uint32_t kRingBulletCount = 24;

  void FireRingVolley();
  void RegisterRingTelegraph();
  void BeginTelegraph();
  void TransitionToRushIfFinished();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
