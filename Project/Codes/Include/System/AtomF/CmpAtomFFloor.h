#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

/// atomF の戦闘床。ローカル XZ の円盤で、半径は 16、Y は 0。囲いは CmpAtomFArenaWall。
class CmpAtomFFloor : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void Draw() override;
};

} // namespace Atrum
