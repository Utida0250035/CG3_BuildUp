#include "System/BehEnemyWallShot.h"
#include "System/BossCombatEnrage.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpEnemy.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/EntityFactory.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"
#include <vector>

namespace Atrum {

void BehEnemyWallShot::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyWallShot::BeginTelegraphPhase() {
  phase_ = Phase::Telegraph;
  telegraphTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
      RefGrandOwner(), kDropAttackTelegraphSeconds);
  telegraphWarningPending_ = true;
}

void BehEnemyWallShot::Initialize() {
  shotCount_ = 0;
  BeginTelegraphPhase();
}

void BehEnemyWallShot::FireVolley() {
  std::vector<float> rapidBulletShotX;
  for (float x = 0.5f + kLeftEndX; x < kRightEndX; x += 6.0f) {
    rapidBulletShotX.push_back(x);
  }
  for (const float shotX : rapidBulletShotX) {
    cmpBullet_->Shot(EntityFactory::LinearEnemyBullet(
                         {0.0f, -rapidBulletSpeed_, 0.0f},
                         {shotX, bulletShotY_, 0.0f})
                         .release());
  }
  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});

  std::vector<float> slowBulletShotX;
  for (float x = 0.5f + kLeftEndX; x < kRightEndX; x += 8.0f) {
    slowBulletShotX.push_back(x);
  }
  if (shotCount_ * 2ULL < slowBulletShotX.size()) {
    slowBulletShotX.erase(slowBulletShotX.begin() +
                          static_cast<std::ptrdiff_t>(shotCount_ * 2ULL));
  }
  for (const float shotX : slowBulletShotX) {
    cmpBullet_->Shot(EntityFactory::LinearEnemyBullet(
                         {0.0f, -slowBulletSpeed_, 0.0f},
                         {shotX, bulletShotY_, 0.0f})
                         .release());
  }
  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});

  ++shotCount_;
}

void BehEnemyWallShot::TransitionToRushIfFinished() {
  const uint32_t shotCountMax =
      BossCombatEnrage::ScaleShotCountMax(RefGrandOwner(), shotCountMax_);
  if (shotCount_ < shotCountMax) {
    return;
  }

  shotCount_ = shotCountMax;
  BehRush *behRush = new BehRush();
  behRush->SetDestinationPos(cmpEnemy_->GetPlayer()->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush);
}

void BehEnemyWallShot::Execute() {
  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Telegraph) {
    if (telegraphWarningPending_) {
      PlayBulletWarningSound();
      telegraphWarningPending_ = false;
    }

    if (CmpAttackTelegraph *telegraph =
            RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
      RegisterWallShotDropTelegraph(telegraph, bulletShotY_, kGroundHeight,
                                    shotCount_);
    }

    telegraphTimer_ -= deltaTime;
    if (telegraphTimer_ > 0.0f) {
      TransitionToRushIfFinished();
      return;
    }

    FireVolley();
    TransitionToRushIfFinished();
    const uint32_t shotCountMax =
        BossCombatEnrage::ScaleShotCountMax(RefGrandOwner(), shotCountMax_);
    if (shotCount_ >= shotCountMax) {
      return;
    }

    phase_ = Phase::IntervalWait;
    intervalTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
        RefGrandOwner(), intervalBetweenVolleys_);
    return;
  }

  intervalTimer_ -= deltaTime;
  if (intervalTimer_ <= 0.0f) {
    BeginTelegraphPhase();
  }

  TransitionToRushIfFinished();
}

} // namespace Atrum
