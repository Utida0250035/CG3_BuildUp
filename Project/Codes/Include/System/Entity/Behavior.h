#pragma once

#include "System/Entity/Entity.h"

namespace Atrum {

class Behavior {
private:
  Entity *grandOwner_ = nullptr;

public:
  virtual ~Behavior() {};
  Behavior() = default;

  virtual void Initialize() {}

  virtual void Execute() {}

  virtual void Finalize() {}

  virtual void ResolveDependence() {}

protected:
  const Entity *GetGrandOwner() const { return grandOwner_; }
  Entity &RefGrandOwner() {

    assert(grandOwner_);

    return *grandOwner_;
  }

  bool isFinished_ = false;

public:
  void SetGrandOwner(Entity *owner) { grandOwner_ = owner; }
  bool GetIsFinish() const { return isFinished_; }
};

} // namespace Atrum