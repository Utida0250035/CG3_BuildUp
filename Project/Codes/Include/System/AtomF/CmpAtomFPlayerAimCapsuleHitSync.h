#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFPlayerAimCapsule;
class CmpHitSphere;

/// エイムカプセル軸上に半径bの球当たりを並べ、自機本体の球と `CmpHitSphere` に合成する。
class CmpAtomFPlayerAimCapsuleHitSync final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_JUDGE;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  CmpAtomFPlayerAimCapsule *capsule_ = nullptr;
  CmpHitSphere *hitSphere_ = nullptr;
};

} // namespace Atrum