#include "System/BehWalkAndJump.h"
#include "Math/Quaternion.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/Playable/CmpInput.h"
#include "System/StageBounds.h"
#include <numbers>

namespace Atrum::Player {

void BehWalkAndJump::ResolveDependence() {
  cmpInput_ = CmpInput::Instance();
  cmpLhRigidBody_ = RefGrandOwner().GetUpdCmp<CmpLhRigidBody>();
  cmpDirection_ = RefGrandOwner().GetDataCmp<CmpDirection>();
}

void BehWalkAndJump::Execute() {
  Math::Vector3 &velocity = cmpLhRigidBody_->RefVelocity();
  const InputData &inputData = cmpInput_->GetData();
  Math::Vector3 &direction = cmpDirection_->RefDirection();
  Math::Quaternion &quaternion = RefGrandOwner().RefTransform().quaternion;

  velocity.x = 0.0f;

  if (inputData.vec2Input.x < 0.0f) {
    velocity.x = -walkSpeed_;
    direction.x = -1.0f;
    quaternion = Math::Quaternion::FromAxisAngle(Math::Vector3::UpLh(), -std::numbers::pi_v<float> * 0.5f);
  } else if (inputData.vec2Input.x > 0.0f) {
    velocity.x = walkSpeed_;
    direction.x = 1.0f;
    quaternion = Math::Quaternion::FromAxisAngle(Math::Vector3::UpLh(), std::numbers::pi_v<float> * 0.5f);
  }

  if (!inputData.isJumpTrigger) {
    return;
  }

  if (RefGrandOwner().GetTransform().translate.y <= kGroundHeight) {
    isAirJumpVariable_ = true;
    velocity.y = jumpSpeedOrigin_;
  } else if (isAirJumpVariable_) {
    velocity.y = jumpSpeedOrigin_;
    isAirJumpVariable_ = false;
  }
}

} // namespace Atrum::Player
