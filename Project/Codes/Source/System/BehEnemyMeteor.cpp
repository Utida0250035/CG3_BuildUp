#include "System/BehEnemyMeteor.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/AttackTelegraph/CmpAttackTelegraph.h"
#include "System/Entity/Components/CmpEnemy.h"
#include "System/Entity/Components/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"

#include <filesystem>
#include <memory>

namespace Atrum {

void BehEnemyMeteor::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyMeteor::Initialize() {
  phase_ = Phase::Telegraph;
  telegraphCount_ = 0;
  shotCount_ = 0;
  timer_ = kIntervalSeconds;

  Entity *player = cmpEnemy_->GetPlayer();
  const float initialX = player != nullptr ? player->GetWorldPosition().x
                                           : RefGrandOwner().GetWorldPosition().x;
  for (float &x : targetXPositions_) {
    x = initialX;
  }

  // 1回目の予告（即座に開始）
  telegraphCount_ = 1;
  telegraphWarningPending_ = true;
}

void BehEnemyMeteor::FireMeteor(const float targetX) {
  cmpBullet_->Shot(EntityFactory::LinearEnemyBullet(
                       {0.0f, -bulletSpeed_, 0.0f},
                       {targetX, bulletShotY_, 0.0f})
                       .release());
  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});
}

void BehEnemyMeteor::TransitionToRush() {
  std::unique_ptr<BehRush> behRush = std::make_unique<BehRush>();
  behRush->SetDestinationPos(cmpEnemy_->GetPlayer()->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush.release());
}

void BehEnemyMeteor::Execute() {
  const float deltaTime = frameDeltaTime_->GetDeltaTime();
  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr) {
    return;
  }

  // 予測線の描画
  if (CmpAttackTelegraph *telegraph =
          RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
    if (phase_ == Phase::Telegraph) {
      for (uint32_t i = 0; i < telegraphCount_; ++i) {
        AddVerticalDropTelegraph(telegraph, targetXPositions_[i], bulletShotY_,
                                 kGroundHeight);
      }
    } else if (phase_ == Phase::RapidFire) {
      for (uint32_t i = shotCount_; i < kMeteorCount; ++i) {
        AddVerticalDropTelegraph(telegraph, targetXPositions_[i], bulletShotY_,
                                 kGroundHeight);
      }
    }
  }

  if (phase_ == Phase::Telegraph) {
    if (telegraphWarningPending_) {
      PlayBulletWarningSound();
      telegraphWarningPending_ = false;
    }

    timer_ -= deltaTime;
    if (timer_ <= 0.0f) {
      if (telegraphCount_ < kMeteorCount) {
        // 次の予告（プレイヤーの現在X座標をロックオンして警告音）
        targetXPositions_[telegraphCount_] = player->GetWorldPosition().x;
        ++telegraphCount_;
        PlayBulletWarningSound();
        timer_ += kIntervalSeconds;
      } else {
        // 全3回の予告が完了 -> 発射フェーズへ
        phase_ = Phase::RapidFire;
        timer_ = 0.0f; // 初弾を直ちに発射
      }
    }
    return;
  }

  if (phase_ == Phase::RapidFire) {
    timer_ -= deltaTime;
    if (timer_ <= 0.0f) {
      if (shotCount_ < kMeteorCount) {
        FireMeteor(targetXPositions_[shotCount_]);
        ++shotCount_;
        timer_ += kIntervalSeconds;
      } else {
        // 3発発射後のインターバル経過で突進へ
        TransitionToRush();
      }
    }
  }
}

} // namespace Atrum
