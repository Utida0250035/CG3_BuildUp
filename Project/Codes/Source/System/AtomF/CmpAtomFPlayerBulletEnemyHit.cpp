#include "System/AtomF/CmpAtomFPlayerBulletEnemyHit.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFPlayerBulletConsume.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFPlayerBulletMove.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum {

void CmpAtomFPlayerBulletEnemyHit::ResolveDependence() {
  hitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
  move_ = RefOwner().GetUpdCmp<CmpAtomFPlayerBulletMove>();
  tags_ = &EntityTagRegistry::Instance();
  tags_->EnsureLoaded();
}

void CmpAtomFPlayerBulletEnemyHit::DestroyBulletAfterHit() {
  if (consumed_) {
    return;
  }
  consumed_ = true;
  AtomF::ConsumePlayerBulletAfterHit(RefOwner());
}

void CmpAtomFPlayerBulletEnemyHit::Update() {
  if (consumed_ || hitSphere_ == nullptr || move_ == nullptr ||
      tags_ == nullptr) {
    return;
  }

  if (!tags_->EntityHasRole(RefOwner(), "playerWeapon")) {
    return;
  }

  const float knockbackSpeed =
      AtomF::ResolveEnemyKnockbackSpeedMetersPerSecond();
  const Math::Vector3 knockDirection = move_->GetDirectionXZUnit();

  for (Entity *other : hitSphere_->GetHitEntitys()) {
    if (other == nullptr || !tags_->EntityHasRole(*other, "enemy")) {
      continue;
    }

    // ボスは吹き飛ばず、弾 1 発ぶんのダメージを受ける。
    if (other->HasUpdCmp<CmpAtomFBoss>()) {
      CmpAtomFBoss *boss = other->GetUpdCmp<CmpAtomFBoss>();
      if (boss != nullptr && boss->IsDamageable()) {
        boss->ApplyDamage(
            AtomF::ParametersRuntime::Instance().Boss().bulletDamage,
            RefOwner().GetWorldPosition());
        DestroyBulletAfterHit();
        return;
      }
      continue;
    }

    CmpAtomFEnemyKnockback *knockback = other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    if (knockback == nullptr) {
      other->AddUpdCmp<CmpAtomFEnemyKnockback>();
      other->ResolveDependencies();
      knockback = other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    }
    if (knockback != nullptr) {
      knockback->ApplyKnockbackAlongBulletDirection(knockDirection,
                                                    knockbackSpeed);
    }

    DestroyBulletAfterHit();
    return;
  }
}

} // namespace Atrum