#include "System/BehEnemyOrbiterSpoke.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/BossCombatEnrage.h"
#include "System/AttackTelegraph/CmpAttackTelegraph.h"
#include "System/Entity/Components/CmpEnemy.h"
#include "System/Entity/Components/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/OrbiterChildUtils.h"
#include "System/PlaySe.h"

namespace Atrum {

namespace {

using OrbiterChildUtils::ClampBossOrbiterDistanceFromBoss;
using OrbiterChildUtils::ForEachBossOrbiterEntity;

Math::Vector3 OutwardFromBossThroughOrbiter(Entity &boss,
                                            Entity &orbiterEntity) {
  Math::Vector3 direction =
      orbiterEntity.GetWorldPosition() - boss.GetWorldPosition();
  if (direction.LengthSquare() <= 1.0e-8f) {
    return Math::Vector3{1.0f, 0.0f, 0.0f};
  }
  return direction.Normalized();
}

} // namespace

void BehEnemyOrbiterSpoke::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyOrbiterSpoke::BeginTelegraph() {
  phase_ = Phase::Telegraph;
  timer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(RefGrandOwner(),
                                                      kTelegraphSeconds);
  warningPending_ = true;
}

void BehEnemyOrbiterSpoke::Initialize() {
  ClampBossOrbiterDistanceFromBoss(RefGrandOwner());
  volleyIndex_ = 0;
  BeginTelegraph();
}

void BehEnemyOrbiterSpoke::FireSpokeVolley() {
  if (cmpBullet_ == nullptr) {
    return;
  }

  Entity &boss = RefGrandOwner();
  ForEachBossOrbiterEntity(boss, [&](Entity &orbiterEntity, size_t, size_t) {
    const Math::Vector3 start = orbiterEntity.GetWorldPosition();
    const Math::Vector3 direction =
        OutwardFromBossThroughOrbiter(boss, orbiterEntity);
    cmpBullet_->Shot(
        EntityFactory::LinearEnemyBullet(direction * kBulletSpeed, start)
            .release());
  });

  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});
}

void BehEnemyOrbiterSpoke::TransitionToRushIfFinished() {
  const uint32_t volleyMax = BossCombatEnrage::ScaleShotCountMax(
      RefGrandOwner(), volleyCountMax_);
  if (volleyIndex_ < volleyMax) {
    return;
  }

  volleyIndex_ = volleyMax;
  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr) {
    return;
  }

  std::unique_ptr<BehRush> behRush = std::make_unique<BehRush>();
  behRush->SetDestinationPos(player->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush.release());
}

void BehEnemyOrbiterSpoke::Execute() {
  ClampBossOrbiterDistanceFromBoss(RefGrandOwner());

  if (frameDeltaTime_ == nullptr || cmpEnemy_ == nullptr) {
    TransitionToRushIfFinished();
    return;
  }

  Entity &boss = RefGrandOwner();
  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Telegraph) {
    if (warningPending_) {
      PlayBulletWarningSound();
      warningPending_ = false;
    }

    if (CmpAttackTelegraph *telegraph = boss.GetDataCmp<CmpAttackTelegraph>()) {
      ForEachBossOrbiterEntity(boss, [&](Entity &orbiterEntity, size_t, size_t) {
        const Math::Vector3 start = orbiterEntity.GetWorldPosition();
        const Math::Vector3 direction =
            OutwardFromBossThroughOrbiter(boss, orbiterEntity);
        telegraph->AddLine(start, ExtendRayWithinStageBounds(start, direction));
      });
    }

    timer_ -= deltaTime;
    if (timer_ > 0.0f) {
      TransitionToRushIfFinished();
      return;
    }

    FireSpokeVolley();
    ++volleyIndex_;
    TransitionToRushIfFinished();

    const uint32_t volleyMax = BossCombatEnrage::ScaleShotCountMax(
        RefGrandOwner(), volleyCountMax_);
    if (volleyIndex_ >= volleyMax) {
      return;
    }

    phase_ = Phase::IntervalWait;
    timer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
        RefGrandOwner(), kIntervalBetweenVolleysSeconds);
    return;
  }

  timer_ -= deltaTime;
  if (timer_ <= 0.0f) {
    BeginTelegraph();
  }
  TransitionToRushIfFinished();
}

} // namespace Atrum
