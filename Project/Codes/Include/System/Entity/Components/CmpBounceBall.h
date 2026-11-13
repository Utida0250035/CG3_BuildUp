#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"
#include <string>

namespace Atrum {

/// 簡易ボール運動＋壁／床バウンス、衝突点でワンショットパーティクル
class CmpBounceBall : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PHYSICS;
  }

  void Initialize() override;
  void Update() override;

  void SetInitialVelocity(const Math::Vector3 &velocity) {
    velocity_ = velocity;
  }
  void SetGravity(const float gravity) { gravity_ = gravity; }
  void SetLeftWallX(const float x) { leftWallX_ = x; }
  void SetRightWallX(const float x) { rightWallX_ = x; }
  void SetGroundY(const float y) { groundY_ = y; }
  void SetRadius(const float radius) { radius_ = radius; }
  void SetWallRestitution(const float value) { wallRestitution_ = value; }
  void SetFloorRestitution(const float value) { floorRestitution_ = value; }
  void SetImpactJsonPath(const std::string &path) { impactJsonPath_ = path; }
  void SetImpactCooldownSeconds(const float seconds) {
    impactCooldownSeconds_ = seconds;
  }

private:
  void SpawnImpactAt(const Math::Vector3 &worldPosition);

  Math::Vector3 velocity_{7.0f, 5.5f, 0.0f};
  float gravity_ = -14.0f;
  float leftWallX_ = -5.0f;
  float rightWallX_ = 5.0f;
  float groundY_ = -3.68f;
  float radius_ = 0.32f;
  float wallRestitution_ = 0.94f;
  float floorRestitution_ = 0.88f;
  std::string impactJsonPath_{"demos/ball_wall_shockwave.json"};
  float impactCooldownSeconds_ = 0.12f;

  float impactCooldownRemaining_ = 0.0f;
  uint32_t impactSequence_ = 0;
};

} // namespace Atrum
