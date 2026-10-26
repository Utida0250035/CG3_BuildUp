#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpReflectAbleBullet;

class CmpDrawBullet : public ComponentDraw {
private:
  CmpReflectAbleBullet *cmpBullet_ = nullptr;

public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::BULLET_VISUAL;
  }

  void ResolveDependence() override;
  void Draw() override;
};

} // namespace Atrum
