#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFPlayerAimCapsule;

/// エイムカプセルの簡易表示（軸線）。
class CmpDrawAtomFPlayerAimCapsule final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void ResolveDependence() override;
  void Draw() override;

private:
  CmpAtomFPlayerAimCapsule *capsule_ = nullptr;
};

} // namespace Atrum