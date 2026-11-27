#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFBoss;

/// atomF ボスの攻撃予兆（床の円・線・召喚位置）を描く。
class CmpDrawAtomFBoss final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::ATTACK_TELEGRAPH;
  }

  void ResolveDependence() override;
  void Draw() override;

private:
  CmpAtomFBoss *boss_ = nullptr;
};

} // namespace Atrum
