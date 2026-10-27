#include "System/BehEnemyBulletCurtainRadialFan.h"

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
#include "Math/Lerp.h"

#include <cmath>

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

void BehEnemyBulletCurtainRadialFan::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyBulletCurtainRadialFan::BeginTelegraph() {
  phase_ = Phase::Telegraph;
  timer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(RefGrandOwner(),
                                                      kTelegraphSeconds);
  warningPending_ = true;
}

void BehEnemyBulletCurtainRadialFan::Initialize() {
  volleyIndex_ = 0;
  BeginTelegraph();
}

void BehEnemyBulletCurtainRadialFan::RegisterFanTelegraph() {
  Entity *player = cmpEnemy_->GetPlayer();
  CmpAttackTelegraph *telegraph =
      RefGrandOwner().GetDataCmp<CmpAttackTelegraph>();
  if (player == nullptr || telegraph == nullptr) {
    return;
  }

  const float planeZ = BattlePlaneZ(cmpEnemy_);
  const Math::Vector3 start =
      SnapToBattlePlane(RefGrandOwner().GetWorldPosition(), planeZ);
  Math::Vector3 toPlayer = player->GetWorldPosition() - start;
  toPlayer.z = 0.0f;
  const float baseAngle = std::atan2(toPlayer.y, toPlayer.x);

  if (kFanTelegraphLineCount <= 1) {
    const Math::Vector3 direction{std::cos(baseAngle), std::sin(baseAngle),
                                  0.0f};
    telegraph->AddLine(start, ExtendRayWithinStageBounds(start, direction));
    return;
  }

  for (uint32_t i = 0; i < kFanTelegraphLineCount; ++i) {
    const float t = static_cast<float>(i) /
                    static_cast<float>(kFanTelegraphLineCount - 1);
    const float angle =
        baseAngle + Math::Lerp(-kHalfFanRadians, kHalfFanRadians, t);
    const Math::Vector3 direction{std::cos(angle), std::sin(angle), 0.0f};
    telegraph->AddLine(start, ExtendRayWithinStageBounds(start, direction));
  }
}

void BehEnemyBulletCurtainRadialFan::FireFanVolley() {
  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr || cmpBullet_ == nullptr) {
    return;
  }

  const float planeZ = BattlePlaneZ(cmpEnemy_);
  const Math::Vector3 start =
      SnapToBattlePlane(RefGrandOwner().GetWorldPosition(), planeZ);
  Math::Vector3 toPlayer = player->GetWorldPosition() - start;
  toPlayer.z = 0.0f;
  const float baseAngle = std::atan2(toPlayer.y, toPlayer.x);

  for (uint32_t i = 0; i < kFanBulletCount; ++i) {
    const float t = kFanBulletCount <= 1
                        ? 0.5f
                        : static_cast<float>(i) /
                              static_cast<float>(kFanBulletCount - 1);
    const float angle =
        baseAngle + Math::Lerp(-kHalfFanRadians, kHalfFanRadians, t);
    const Math::Vector3 direction{std::cos(angle), std::sin(angle), 0.0f};
    cmpBullet_->Shot(
        EntityFactory::LinearEnemyBullet(direction * kBulletSpeed, start)
            .release());
  }

  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});
}

void BehEnemyBulletCurtainRadialFan::TransitionToRushIfFinished() {
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

void BehEnemyBulletCurtainRadialFan::Execute() {
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

    RegisterFanTelegraph();

    timer_ -= deltaTime;
    if (timer_ > 0.0f) {
      TransitionToRushIfFinished();
      return;
    }

    FireFanVolley();
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
