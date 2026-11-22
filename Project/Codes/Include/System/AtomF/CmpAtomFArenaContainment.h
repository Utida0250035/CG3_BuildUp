#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF で自機・弾・敵などを円形アリーナ内に留める。当たり半径の余白を bodyRadiusMeters で指定。
class CmpAtomFArenaContainment final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PHYSICS;
  }

  void Update() override;

  void SetBodyRadiusMeters(const float bodyRadiusMeters) {
    bodyRadiusMeters_ = bodyRadiusMeters;
  }

  float GetBodyRadiusMeters() const { return bodyRadiusMeters_; }

private:
  float bodyRadiusMeters_ = 0.25f;
};

} // namespace Atrum