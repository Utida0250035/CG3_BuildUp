#include "System/BehEnemyShot.h"
#include "System/BossCombatEnrage.h"
#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpEnemy.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/PlaySe.h"

namespace Atrum {

void BehEnemyShot::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyShot::Initialize() {
  shotCoolTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
      RefGrandOwner(), shotCoolTime_);
  shotCount_ = 0;
  bulletWarningPending_ = true;
}

void BehEnemyShot::Execute() {
  const uint32_t shotCountMax =
      BossCombatEnrage::ScaleShotCountMax(RefGrandOwner(), shotCountMax_);

  if (shotCoolTimer_ <= 0.0f) {
    shotCoolTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
        RefGrandOwner(), shotCoolTime_);
    const Math::Vector3 direction =
        (cmpEnemy_->GetPlayer()->GetWorldPosition() - RefGrandOwner().GetWorldPosition()).Normalized();
    cmpBullet_->Shot(EntityFactory::LinearEnemyBullet(direction * bulletSpeed_, RefGrandOwner().GetTransform().translate).release());
    ++shotCount_;
    PlaySe("./Resources/seEnemyShot.mp3");

    if (shotCount_ < shotCountMax) {
      bulletWarningPending_ = true;
    }
  } else {
    if (bulletWarningPending_) {
      PlayBulletWarningSound();
      bulletWarningPending_ = false;
    }

    if (Entity *player = cmpEnemy_->GetPlayer()) {
      if (CmpAttackTelegraph *telegraph =
              RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
        const Math::Vector3 start = RefGrandOwner().GetWorldPosition();
        const Math::Vector3 direction =
            (player->GetWorldPosition() - start).Normalized();
        telegraph->AddLine(start,
                           ExtendRayWithinStageBounds(start, direction));
      }
    }

    shotCoolTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (shotCoolTimer_ < 0.0f) {
      shotCoolTimer_ = 0.0f;
    }
  }

  if (shotCount_ < shotCountMax) {
    return;
  }

  shotCount_ = shotCountMax;
  BehRush *behRush = new BehRush();
  behRush->SetDestinationPos(cmpEnemy_->GetPlayer()->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush);
}

} // namespace Atrum
