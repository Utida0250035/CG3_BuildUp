#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFEnemyKnockbackPredictCapsule;

/// 敵の吹き飛び予測カプセル軸線表示。
class CmpDrawAtomFEnemyKnockbackPredictCapsule final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void ResolveDependence() override;
  void Draw() override;

private:
  CmpAtomFEnemyKnockbackPredictCapsule *capsule_ = nullptr;
};

} // namespace Atrum
