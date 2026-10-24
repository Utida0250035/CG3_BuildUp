#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

namespace Player {
class CmpInput;
}

class CmpLhRigidBody;
class CmpDirection;

namespace Player {

class BehWalkAndJump : public Behavior {
private:
  Player::CmpInput *cmpInput_ = nullptr;
  CmpLhRigidBody *cmpLhRigidBody_ = nullptr;
  CmpDirection *cmpDirection_ = nullptr;
  float walkSpeed_ = 6.0f;
  float jumpSpeedOrigin_ = 16.0f;
  bool isAirJumpVariable_ = false;

public:
  void ResolveDependence() override;
  void Execute() override;
};

} // namespace Player

} // namespace Atrum
