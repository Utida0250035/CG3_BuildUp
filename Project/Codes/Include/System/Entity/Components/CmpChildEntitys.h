#pragma once

#include "System/Entity/ComponentUpd.h"
#include "System/Entity/Entity.h"
#include <memory>
#include <vector>

namespace Atrum {

class CmpChildEntitys : public ComponentUpd {
private:
  std::vector<std::unique_ptr<Entity>> childs_{};

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::NONE_GROUPING; }

  void Update() override {
    size_t index = 0;
    while (index < childs_.size()) {
      if (!childs_[index] || !childs_[index]->GetParent()) {
        childs_.erase(childs_.begin() + static_cast<std::ptrdiff_t>(index));
        continue;
      }
      ++index;
    }
  }

  std::vector<std::unique_ptr<Entity>> &RefChilds() { return childs_; }

  void AddChilds(const std::vector<Entity *> &childs) {
    for (Entity *child : childs) {
      child->SetParent(&RefOwner());
      childs_.emplace_back(child);
    }
  }
};

} // namespace Atrum
