#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAttackTelegraph;

class CmpDrawAttackTelegraph : public ComponentDraw {
private:
  CmpAttackTelegraph *cmpAttackTelegraph_ = nullptr;

public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::ATTACK_TELEGRAPH;
  }

  void ResolveDependence() override;
  void Draw() override;
};

} // namespace Atrum
