#include "System/BehEnemyMeteor.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpEnemy.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"

namespace Atrum {

void BehEnemyMeteor::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyMeteor::BeginTelegraphPhase() {
  phase_ = Phase::Telegraph;
  telegraphTimer_ = kDropAttackTelegraphSeconds;
}

void BehEnemyMeteor::Initialize() {
  shotCount_ = 0;
  BeginTelegraphPhase();
}

void BehEnemyMeteor::FireMeteor() {
  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr) {
    return;
  }

  cmpBullet_->Shot(EntityFactory::LinearEnemyBullet(
                       {0.0f, -bulletSpeed_, 0.0f},
                       {player->GetWorldPosition().x, bulletShotY_, 0.0f})
                       .release());
  ++shotCount_;
  PlaySe("./Resources/seEnemyShot.mp3");
}

void BehEnemyMeteor::TransitionToRushIfFinished() {
  if (shotCount_ < shotCountMax_) {
    return;
  }

  shotCount_ = shotCountMax_;
  BehRush *behRush = new BehRush();
  behRush->SetDestinationPos(cmpEnemy_->GetPlayer()->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush);
}

void BehEnemyMeteor::Execute() {
  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Telegraph) {
    if (Entity *player = cmpEnemy_->GetPlayer()) {
      if (CmpAttackTelegraph *telegraph =
              RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
        AddVerticalDropTelegraph(telegraph, player->GetWorldPosition().x,
                                 bulletShotY_, kGroundHeight);
      }
    }

    telegraphTimer_ -= deltaTime;
    if (telegraphTimer_ > 0.0f) {
      TransitionToRushIfFinished();
      return;
    }

    FireMeteor();
    TransitionToRushIfFinished();
    if (shotCount_ >= shotCountMax_) {
      return;
    }

    phase_ = Phase::IntervalWait;
    intervalTimer_ = intervalAfterShot_;
    return;
  }

  intervalTimer_ -= deltaTime;
  if (intervalTimer_ <= 0.0f) {
    BeginTelegraphPhase();
  }

  TransitionToRushIfFinished();
}

} // namespace Atrum
