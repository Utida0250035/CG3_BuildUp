#pragma once

#include "System/Entity/BehaviorBox.h"
#include "System/Entity/ComponentUpd.h"
#include <memory>

namespace Atrum {

class CmpBehavior : public ComponentUpd {
private:
  BehaviorBox behaviorBox_{};
  std::unique_ptr<Behavior> (*initialBehaviorFactory_)() = nullptr;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::MOVEMENT; }

  void ResolveDependence() override { behaviorBox_.SetGrandOwner(&RefOwner()); }

  void Initialize() override {
    behaviorBox_.Transition(initialBehaviorFactory_().release());
  }

  void Update() override { behaviorBox_.Execute(); }

  template <typename T> void SetInitialBehavior() {
    initialBehaviorFactory_ = []() { return std::unique_ptr<Behavior>(std::make_unique<T>()); };
  }
};

} // namespace Atrum
