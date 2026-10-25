#pragma once

#include "System/DeltaTime.h"
#include "System/Entity/Behavior.h"
#include <memory>

namespace Atrum {

class CmpDirection;
class CmpLhRigidBody;
class BehaviorBox;

class BehDodge : public Behavior {
private:
  CmpDirection *cmpDirection_ = nullptr;
  CmpLhRigidBody *cmpLhRigidBody_ = nullptr;
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  std::unique_ptr<Behavior> nextBehavior_ = nullptr;
  BehaviorBox *pBehaviorBox_ = nullptr;

  float dodgeSpeed_ = 24.0f;
  float dodgeTime_ = 0.125f;
  float dodgeTimer_ = 0.0f;

public:
  void Initialize() override;
  void Execute() override;
  void Finalize() override;
  void ResolveDependence() override;

  void SetPBehaviorBox(BehaviorBox *pBehaviorBox) { pBehaviorBox_ = pBehaviorBox; }
  void SetNextBehavior(Behavior *nextBehavior) { nextBehavior_.reset(nextBehavior); }
};

} // namespace Atrum
