#include "System/BehEnemyOrbiterSalvo.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/BossCombatEnrage.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpEnemy.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/OrbiterChildUtils.h"
#include "System/PlaySe.h"

#include <cmath>

namespace Atrum {

namespace {

using OrbiterChildUtils::ClampBossOrbiterDistanceFromBoss;
using OrbiterChildUtils::CountBossOrbiters;
using OrbiterChildUtils::ForEachBossOrbiterEntity;

void AddOrbiterAimTelegraph(Entity &boss, Entity &orbiterEntity, Entity *player) {
  if (player == nullptr) {
    return;
  }

  CmpAttackTelegraph *telegraph = boss.GetDataCmp<CmpAttackTelegraph>();
  if (telegraph == nullptr) {
    return;
  }

  const Math::Vector3 start = orbiterEntity.GetWorldPosition();
  const Math::Vector3 toPlayer =
      (player->GetWorldPosition() - start).Normalized();
  telegraph->AddLine(start, ExtendRayWithinStageBounds(start, toPlayer));
}

} // namespace

void BehEnemyOrbiterSalvo::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyOrbiterSalvo::BeginVolleyTelegraph() {
  phase_ = Phase::VolleyTelegraph;
  timer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
      RefGrandOwner(), kVolleyTelegraphSeconds);
  orbiterFireIndex_ = 0;
  warningPending_ = true;
}

void BehEnemyOrbiterSalvo::Initialize() {
  ClampBossOrbiterDistanceFromBoss(RefGrandOwner());
  orbiterCount_ = CountBossOrbiters(RefGrandOwner());
  volleyIndex_ = 0;
  BeginVolleyTelegraph();
}

void BehEnemyOrbiterSalvo::FireFromOrbiterIndex(const size_t index) {
  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr || cmpBullet_ == nullptr) {
    return;
  }

  ForEachBossOrbiterEntity(
      RefGrandOwner(),
      [&](Entity &orbiterEntity, size_t orbiterIndex, size_t) {
        if (orbiterIndex != index) {
          return;
        }

        const Math::Vector3 start = orbiterEntity.GetWorldPosition();
        Math::Vector3 direction =
            player->GetWorldPosition() - start;
        if (direction.LengthSquare() <= 1.0e-8f) {
          direction = Math::Vector3{1.0f, 0.0f, 0.0f};
        } else {
          direction = direction.Normalized();
        }

        cmpBullet_->Shot(
            EntityFactory::LinearEnemyBullet(direction * kBulletSpeed, start)
                .release());
      });

  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});
}

void BehEnemyOrbiterSalvo::TransitionToRushIfFinished() {
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

  BehRush *behRush = new BehRush();
  behRush->SetDestinationPos(player->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush);
}

void BehEnemyOrbiterSalvo::Execute() {
  ClampBossOrbiterDistanceFromBoss(RefGrandOwner());

  if (frameDeltaTime_ == nullptr || cmpEnemy_ == nullptr ||
      orbiterCount_ == 0) {
    TransitionToRushIfFinished();
    return;
  }

  Entity *player = cmpEnemy_->GetPlayer();
  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::VolleyTelegraph) {
    if (warningPending_) {
      PlayBulletWarningSound();
      warningPending_ = false;
    }

    if (player != nullptr) {
      ForEachBossOrbiterEntity(RefGrandOwner(),
                               [&](Entity &orbiterEntity, size_t, size_t) {
                                 AddOrbiterAimTelegraph(RefGrandOwner(),
                                                        orbiterEntity, player);
                               });
    }

    timer_ -= deltaTime;
    if (timer_ > 0.0f) {
      TransitionToRushIfFinished();
      return;
    }

    phase_ = Phase::OrbiterTelegraph;
    timer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
        RefGrandOwner(), kPerOrbiterTelegraphSeconds);
    warningPending_ = true;
    return;
  }

  if (phase_ == Phase::OrbiterTelegraph) {
    if (orbiterFireIndex_ < orbiterCount_) {
      if (warningPending_) {
        PlayBulletWarningSound();
        warningPending_ = false;
      }

      if (player != nullptr) {
        ForEachBossOrbiterEntity(
            RefGrandOwner(),
            [&](Entity &orbiterEntity, size_t orbiterIndex, size_t) {
              if (orbiterIndex == orbiterFireIndex_) {
                AddOrbiterAimTelegraph(RefGrandOwner(), orbiterEntity, player);
              }
            });
      }

      timer_ -= deltaTime;
      if (timer_ > 0.0f) {
        TransitionToRushIfFinished();
        return;
      }

      FireFromOrbiterIndex(orbiterFireIndex_);
      ++orbiterFireIndex_;

      if (orbiterFireIndex_ < orbiterCount_) {
        timer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
            RefGrandOwner(), kPerOrbiterTelegraphSeconds);
        warningPending_ = true;
        TransitionToRushIfFinished();
        return;
      }
    }

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

  if (phase_ == Phase::IntervalWait) {
    timer_ -= deltaTime;
    if (timer_ <= 0.0f) {
      BeginVolleyTelegraph();
    }
    TransitionToRushIfFinished();
  }
}

} // namespace Atrum
