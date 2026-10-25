#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;

class BehEnemyBurstShot : public Behavior {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  float shotCoolTimer_ = 0.0f;
  float shotCoolTime_ = 0.35f;
  float bulletSpeed_ = 9.0f;
  uint32_t shotCount_ = 0;
  uint32_t shotCountMax_ = 3;
  bool bulletWarningPending_ = false;

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum

