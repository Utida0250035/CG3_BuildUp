#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFPlayerBulletTrail;
class EntityTagRegistry;

/// atomF 自機弾が円形アリーナの壁に当たったら弾を消す（軌道は trail が残留）。
class CmpAtomFPlayerBulletArenaWallHit final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_RESPONSE;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  CmpAtomFPlayerBulletTrail *trail_ = nullptr;
  EntityTagRegistry *tags_ = nullptr;
  bool consumed_ = false;
};

} // namespace Atrum