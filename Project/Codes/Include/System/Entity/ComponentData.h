#pragma once

#include <cassert>

namespace Atrum {

class Entity;

// データ専用の全てのパーツの基底
class ComponentData {
private:
  Entity *owner_ = nullptr;

public:
  virtual ~ComponentData() = default;

public:
  void SetOwner(Entity *owner) { owner_ = owner; }
  const Entity *GetOwner() { return owner_; }
  Entity &RefOwner() {
    assert(owner_);

    return *owner_;
  }

  virtual void DevelopGui() {};
};

} // namespace Atrum