#include "System/BehEnemySpreadShot.h"
#include "System/BossCombatEnrage.h"
#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/AttackTelegraph/CmpAttackTelegraph.h"
#include "System/Entity/Components/CmpEnemy.h"
#include "System/Entity/Components/CmpReflectAbleBullet.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/PlaySe.h"

#include <cmath>

namespace Atrum {

void BehEnemySpreadShot::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemySpreadShot::Initialize() {
  shotCoolTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
      RefGrandOwner(), shotCoolTime_);
  shotCount_ = 0;
  bulletWarningPending_ = true;
}

void BehEnemySpreadShot::Execute() {
  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr) {
    return;
  }

  const Math::Vector3 start = RefGrandOwner().GetWorldPosition();
  const Math::Vector3 toPlayer = player->GetWorldPosition() - start;
  const float baseAngle = std::atan2(toPlayer.y, toPlayer.x);

  const float angles[3] = {
      baseAngle,
      baseAngle + spreadAngleRadians_,
      baseAngle - spreadAngleRadians_,
  };

  const uint32_t shotCountMax =
      BossCombatEnrage::ScaleShotCountMax(RefGrandOwner(), shotCountMax_);

  if (shotCoolTimer_ <= 0.0f) {
    shotCoolTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
        RefGrandOwner(), shotCoolTime_);

    for (const float angle : angles) {
      const Math::Vector3 direction{std::cos(angle), std::sin(angle), 0.0f};
      cmpBullet_->Shot(
          EntityFactory::LinearEnemyBullet(
              direction * bulletSpeed_, RefGrandOwner().GetTransform().translate)
              .release());
    }

    ++shotCount_;
    PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                      "./Resources/seEnemyShot.mp3"});

    if (shotCount_ < shotCountMax) {
      bulletWarningPending_ = true;
    }
  } else {
    if (bulletWarningPending_) {
      PlayBulletWarningSound();
      bulletWarningPending_ = false;
    }

    if (CmpAttackTelegraph *telegraph =
            RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
      for (const float angle : angles) {
        const Math::Vector3 direction{std::cos(angle), std::sin(angle), 0.0f};
        telegraph->AddLine(start, ExtendRayWithinStageBounds(start, direction));
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
  behRush->SetDestinationPos(player->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush);
}

} // namespace Atrum

