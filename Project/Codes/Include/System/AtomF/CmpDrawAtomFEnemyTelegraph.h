#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpAtomFEnemyPeriodicCharge;
class CmpAtomFEnemyShooter;

/// atomF 雑魚の攻撃予告。突進の予備動作中は進路の帯を、射撃型の狙い中は照準線を地面に描く。
class CmpDrawAtomFEnemyTelegraph final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void ResolveDependence() override;
  void Draw() override;

private:
  CmpAtomFEnemyPeriodicCharge *charge_ = nullptr;
  CmpAtomFEnemyShooter *shooter_ = nullptr;
};

} // namespace Atrum
