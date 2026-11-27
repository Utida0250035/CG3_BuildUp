#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFBoss;

/// atomF ボスの攻撃予告を地面に描く（弾幕の放射線・召喚位置・叩きつけの予告円・突進の進路・怒りの波紋）。
class CmpDrawAtomFBossTelegraph final : public ComponentDraw {
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
