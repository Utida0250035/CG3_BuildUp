#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

/// 自機方向を中心に扇状の弾幕を一斉発射（弾間の連射なし）
class BehEnemyBulletCurtainRadialFan : public Behavior {
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

  static constexpr float kTelegraphSeconds = 0.75f;
  static constexpr float kIntervalBetweenVolleysSeconds = 1.0f;
  static constexpr float kBulletSpeed = 6.5f;
  static constexpr float kHalfFanRadians = 0.55f; // 約±31度
  static constexpr uint32_t kFanBulletCount = 15;
  /// 予測線は実弾より本数を減らして扇の幅だけ伝える
  static constexpr uint32_t kFanTelegraphLineCount = 5;

  void FireFanVolley();
  void RegisterFanTelegraph();
  void BeginTelegraph();
  void TransitionToRushIfFinished();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
