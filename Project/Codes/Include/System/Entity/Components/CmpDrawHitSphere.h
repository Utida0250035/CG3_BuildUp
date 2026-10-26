#pragma once

#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpHitSphere;

class CmpDrawHitSphere : public ComponentDraw {
private:
  CmpHitSphere *cmpHitSphere_ = nullptr;
  Math::Vector4 colorVec_ = {0.2f, 0.85f, 1.0f, 1.0f};
  bool isLightingEnabled_ = false;

public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::HIT_SPHERE_VISUAL;
  }

  void ResolveDependence() override;
  void Draw() override;

  void SetColor(const Math::Vector4 &color) { colorVec_ = color; }
  void SetLightingEnabled(const bool enabled) { isLightingEnabled_ = enabled; }
};

} // namespace Atrum
