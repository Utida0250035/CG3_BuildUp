#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFPlayerBulletMove;
class CmpHitSphere;
class EntityTagRegistry;

/// atomF 自機弾が敵に当たったとき弾を消し、弾進行方向へ速さbで吹き飛ばす。
class CmpAtomFPlayerBulletEnemyHit final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_RESPONSE;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  void DestroyBulletAfterHit();

  CmpHitSphere *hitSphere_ = nullptr;
  CmpAtomFPlayerBulletMove *move_ = nullptr;
  EntityTagRegistry *tags_ = nullptr;
  bool consumed_ = false;
};

} // namespace Atrum