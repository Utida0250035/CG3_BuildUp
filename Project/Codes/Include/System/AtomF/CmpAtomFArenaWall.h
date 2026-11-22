#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

/// atomF の戦闘エリアを囲む壁。ローカル XZ で内径は 16、床の Y=0 から立ち上がる。出入りの制限は別タスク。
class CmpAtomFArenaWall : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void Draw() override;
};

} // namespace Atrum
