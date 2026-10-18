#pragma once
#include "System/Entity/Behavior.h"
#include <cstdint>
#include <memory>

namespace Atrum {

class Entity;

class BehaviorBox {
private:
  Entity *grandOwner_ = nullptr;
  std::unique_ptr<Behavior> behavior_ = nullptr;
  uint64_t transitionCount_ = 0;

public:
  void Transition(Behavior *behavior);
  void Execute();
  void Finalize();

  void SetGrandOwner(Entity *entity);
  Behavior &RefBehavior() { return *behavior_; }

  uint64_t GetTransitionCount() const { return transitionCount_; }
  void InitializeTransitionCount() { transitionCount_ = 0; }
};

} // namespace Atrum