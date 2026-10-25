#pragma once

#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpMeshSphere : public ComponentDraw {
private:
  Math::Vector4 colorVec_ = Math::Vector4::White();
  float radius_ = 1.0f;
  uint32_t subdivision_ = 8;
  bool isLightingEnabled_ = true;

public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void Draw() override;

  void SetColor(const Math::Vector4 &color) { colorVec_ = color; }
  void SetRadius(const float radius) { radius_ = radius; }
  void SetSubdivision(const uint32_t subdivision) {
    subdivision_ = subdivision;
  }

  void SetLightingEnabled(const bool enabled) { isLightingEnabled_ = enabled; }
};

} // namespace Atrum
