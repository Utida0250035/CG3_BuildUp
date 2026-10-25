#pragma once

#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"

namespace Atrum::Player {

/// パリィ受付中に自機中心へ表示する半透明の球
class CmpDrawPlayerParryField : public ComponentDraw {
private:
  bool isVisible_ = false;
  float radius_ = 2.0f;
  Math::Vector4 color_{0.42f, 0.88f, 1.0f, 0.38f};

public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::HIT_SPHERE_VISUAL;
  }

  void Draw() override;

  void SetVisible(const bool visible) { isVisible_ = visible; }
  void SetRadius(const float radius) { radius_ = radius; }
  void SetColor(const Math::Vector4 &color) { color_ = color; }
};

} // namespace Atrum::Player
