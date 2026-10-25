#include "System/BehDodge.h"
#include "System/DeltaTime.h"
#include "System/Entity/BehaviorBox.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void BehDodge::Initialize() {
  cmpLhRigidBody_->RefVelocity() = {};
  dodgeTimer_ = dodgeTime_;
}

void BehDodge::Execute() {
  if (isFinished_) {
    return;
  }

  cmpLhRigidBody_->RefVelocity() = cmpDirection_->GetDirection() * dodgeSpeed_;
  dodgeTimer_ -= frameDeltaTime_->GetDeltaTime();

  if (dodgeTimer_ > 0.0f) {
    return;
  }

  dodgeTimer_ = 0.0f;
  isFinished_ = true;
  pBehaviorBox_->Transition(nextBehavior_.release());
}

void BehDodge::Finalize() { cmpLhRigidBody_->RefVelocity() = {}; }

void BehDodge::ResolveDependence() {
  cmpDirection_ = RefGrandOwner().GetDataCmp<CmpDirection>();
  cmpLhRigidBody_ = RefGrandOwner().GetUpdCmp<CmpLhRigidBody>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

} // namespace Atrum
