#include "System/Entity/BehaviorBox.h"

#include "System/Entity/Behavior.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void BehaviorBox::ApplyPendingTransitionIfAny() {
  if (!pendingTransition_) {
    return;
  }

  if (behavior_) {
    behavior_->Finalize();
  }

  behavior_ = std::move(pendingTransition_);

  assert(grandOwner_);

  behavior_->SetGrandOwner(grandOwner_);
  behavior_->ResolveDependence();

  behavior_->Initialize();

  transitionCount_++;
}

void BehaviorBox::Transition(Behavior* newBehavior) {
  std::unique_ptr<Behavior> next(newBehavior);
  assert(next);

  // Behavior::Execute() 実行中は this（現在の Behavior）を即座に破棄しない
  if (isExecutingBehavior_ && behavior_) {
    assert(!pendingTransition_ && "同一ティックで複数の遷移は未対応です");
    pendingTransition_ = std::move(next);
    return;
  }

  if (behavior_) {
    behavior_->Finalize();
  }

  behavior_ = std::move(next);

  assert(grandOwner_);

  behavior_->SetGrandOwner(grandOwner_);
  behavior_->ResolveDependence();

  behavior_->Initialize();

  transitionCount_++;
}

void BehaviorBox::Execute() {
  ApplyPendingTransitionIfAny();

  if (!behavior_) {
    return;
  }

  isExecutingBehavior_ = true;
  behavior_->Execute();
  isExecutingBehavior_ = false;

  ApplyPendingTransitionIfAny();
}

void BehaviorBox::Finalize() {
  pendingTransition_.reset();

  if (!behavior_) {
    return;
  }

  behavior_->Finalize();
}

void BehaviorBox::SetGrandOwner(Entity* entity) { grandOwner_ = entity; }

} // namespace Atrum