#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFPlayerBulletTrail;

/// atomF 自機弾軌道の線描画。
class CmpDrawAtomFPlayerBulletTrail final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void ResolveDependence() override;
  void Draw() override;

private:
  CmpAtomFPlayerBulletTrail *trail_ = nullptr;
};

} // namespace Atrum