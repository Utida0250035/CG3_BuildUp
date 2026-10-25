#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

class BehEnemySpreadShot : public Behavior {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  float shotCoolTimer_ = 0.0f;
  float shotCoolTime_ = 1.2f;
  float bulletSpeed_ = 7.5f;
  float spreadAngleRadians_ = 0.314159f; // 約18度
  uint32_t shotCount_ = 0;
  uint32_t shotCountMax_ = 2;
  bool bulletWarningPending_ = false;

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum

