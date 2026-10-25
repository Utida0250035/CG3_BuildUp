#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpReflectAbleBullet;
class CmpEnemy;
class FrameDeltaTime;
class CmpRotator;

class BehEnemySwingShot : public Behavior {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpReflectAbleBullet *cmpBullet_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;
  CmpRotator *cmpRotator_ = nullptr;

  float shotCoolTimer_ = 0.0f;
  float shotCoolTime_ = 0.75f;
  float rapidBulletSpeed_ = 1.0f;
  uint32_t shotCount_ = 0;
  uint32_t shotCountMax_ = 15;

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
};

} // namespace Atrum
