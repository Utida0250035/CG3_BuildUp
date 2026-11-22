#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

/// atomF の戦闘床。ローカル XZ 平面で、Y は 0。円形の半径は別タスク。
class CmpAtomFFloor : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void Draw() override;
};

} // namespace Atrum
