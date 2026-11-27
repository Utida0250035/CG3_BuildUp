#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFBoss;

/// ボスの予備動作（突進の進路・召喚位置・衝撃波の範囲）と頭上の Hp バーを描く。
class CmpDrawAtomFBoss final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void ResolveDependence() override;
  void Draw() override;

private:
  CmpAtomFBoss *boss_ = nullptr;
};

} // namespace Atrum
