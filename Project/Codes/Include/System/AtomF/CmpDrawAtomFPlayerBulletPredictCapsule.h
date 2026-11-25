#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFPlayerBulletPredictCapsule;

/// 自機弾の予測カプセル軸線表示。
class CmpDrawAtomFPlayerBulletPredictCapsule final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void ResolveDependence() override;
  void Draw() override;

private:
  CmpAtomFPlayerBulletPredictCapsule *capsule_ = nullptr;
};

} // namespace Atrum
