#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFPlayerAim;
class CmpAtomFPlayerShotCooldown;
class CmpChildEntitys;

/// atomF 自機の射撃入力（Space / パッド A）。エイム方向へ弾を出す。
class CmpAtomFPlayerFire final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_JUDGE;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  CmpAtomFPlayerAim *aim_ = nullptr;
  CmpAtomFPlayerShotCooldown *shotCooldown_ = nullptr;
  CmpChildEntitys *childHolder_ = nullptr;
};

} // namespace Atrum