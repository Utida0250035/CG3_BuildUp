#include "System/BehEnemyBulletCurtainFixedRing.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/BossCombatEnrage.h"
#include "System/AttackTelegraph/CmpAttackTelegraph.h"
#include "System/Entity/Components/CmpEnemy.h"
#include "System/Entity/Components/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/PlaySe.h"

#include <cmath>
#include <numbers>

namespace Atrum {

namespace {

float BattlePlaneZ(const CmpEnemy *cmpEnemy) {
  if (cmpEnemy == nullptr) {
    return 0.0f;
  }
  const Entity *player = cmpEnemy->GetPlayer();
  return player != nullptr ? player->GetWorldPosition().z : 0.0f;
}

Math::Vector3 SnapToBattlePlane(const Math::Vector3 &worldPosition,
                                const float planeZ) {
  Math::Vector3 onPlane = worldPosition;
  onPlane.z = planeZ;
  return onPlane;
}

} // namespace

void BehEnemyBulletCurtainFixedRing::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyBulletCurtainFixedRing::BeginTelegraph() {
  phase_ = Phase::Telegraph;
  timer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(RefGrandOwner(),
                                                      kTelegraphSeconds);
  warningPending_ = true;

  Entity *player = cmpEnemy_->GetPlayer();
  if (player != nullptr) {
    const float planeZ = BattlePlaneZ(cmpEnemy_);
    const Math::Vector3 start =
        SnapToBattlePlane(RefGrandOwner().GetWorldPosition(), planeZ);
    Math::Vector3 toPlayer = player->GetWorldPosition() - start;
    toPlayer.z = 0.0f;
    ringBaseAngleRadians_ = std::atan2(toPlayer.y, toPlayer.x);
  } else {
    ringBaseAngleRadians_ = 0.0f;
  }
}

void BehEnemyBulletCurtainFixedRing::Initialize() {
  volleyIndex_ = 0;
  BeginTelegraph();
}

void BehEnemyBulletCurtainFixedRing::RegisterRingTelegraph() {
  CmpAttackTelegraph *telegraph =
      RefGrandOwner().GetDataCmp<CmpAttackTelegraph>();
  if (telegraph == nullptr || kRingBulletCount == 0) {
    return;
  }

  const float planeZ = BattlePlaneZ(cmpEnemy_);
  const Math::Vector3 start =
      SnapToBattlePlane(RefGrandOwner().GetWorldPosition(), planeZ);
  const float step = (2.0f * std::numbers::pi_v<float>) /
                     static_cast<float>(kRingBulletCount);

  for (uint32_t i = 0; i < kRingBulletCount; ++i) {
    const float angle = ringBaseAngleRadians_ + step * static_cast<float>(i);
    const Math::Vector3 direction{std::cos(angle), std::sin(angle), 0.0f};
    telegraph->AddLine(start, ExtendRayWithinStageBounds(start, direction));
  }
}

void BehEnemyBulletCurtainFixedRing::FireRingVolley() {
  if (cmpBullet_ == nullptr || kRingBulletCount == 0) {
    return;
  }

  const float planeZ = BattlePlaneZ(cmpEnemy_);
  const Math::Vector3 start =
      SnapToBattlePlane(RefGrandOwner().GetWorldPosition(), planeZ);
  const float step = (2.0f * std::numbers::pi_v<float>) /
                     static_cast<float>(kRingBulletCount);

  for (uint32_t i = 0; i < kRingBulletCount; ++i) {
    const float angle = ringBaseAngleRadians_ + step * static_cast<float>(i);
    const Math::Vector3 direction{std::cos(angle), std::sin(angle), 0.0f};
    cmpBullet_->Shot(
        EntityFactory::LinearEnemyBullet(direction * kBulletSpeed, start)
            .release());
  }

  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});
}

void BehEnemyBulletCurtainFixedRing::TransitionToRushIfFinished() {
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

void BehEnemyBulletCurtainFixedRing::Execute() {
  if (frameDeltaTime_ == nullptr || cmpEnemy_ == nullptr) {
    TransitionToRushIfFinished();
    return;
  }

  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Telegraph) {
    if (warningPending_) {
      PlayBulletWarningSound();
      warningPending_ = false;
    }

    RegisterRingTelegraph();

    timer_ -= deltaTime;
    if (timer_ > 0.0f) {
      TransitionToRushIfFinished();
      return;
    }

    FireRingVolley();
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
