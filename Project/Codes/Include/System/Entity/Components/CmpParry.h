#pragma once

#include "System/Entity/ComponentData.h"
#include "System/Entity/Entity.h"
#include <memory>

namespace Atrum {

class CmpParry : public ComponentData {
private:
  std::unique_ptr<Entity> entParry_ = nullptr;

public:
  void SetEntParry(Entity *entParry) { entParry_.reset(entParry); }
  bool HasEntParry() const { return entParry_ != nullptr; }
  Entity &RefEntParry() { return *entParry_; }
};

} // namespace Atrum
