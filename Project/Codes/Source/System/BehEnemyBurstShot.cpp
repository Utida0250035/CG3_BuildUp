#include "System/BehEnemyBurstShot.h"
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

void BehEnemyBurstShot::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyBurstShot::Initialize() {
  constexpr float kInitialTelegraphWait = 0.6f;
  shotCoolTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
      RefGrandOwner(), kInitialTelegraphWait);
  shotCount_ = 0;
  bulletWarningPending_ = true;
}

void BehEnemyBurstShot::Execute() {
  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr) {
    return;
  }

  const uint32_t shotCountMax =
      BossCombatEnrage::ScaleShotCountMax(RefGrandOwner(), shotCountMax_);

  if (shotCoolTimer_ <= 0.0f) {
    shotCoolTimer_ = BossCombatEnrage::ScaleShotCoolTimeSeconds(
        RefGrandOwner(), shotCoolTime_);
    const Math::Vector3 direction =
        (player->GetWorldPosition() - RefGrandOwner().GetWorldPosition()).Normalized();
    cmpBullet_->Shot(
        EntityFactory::LinearEnemyBullet(
            direction * bulletSpeed_, RefGrandOwner().GetTransform().translate)
            .release());
    ++shotCount_;
    PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                      "./Resources/seEnemyShot.mp3"});
  } else {
    if (bulletWarningPending_) {
      PlayBulletWarningSound();
      bulletWarningPending_ = false;
    }

    if (CmpAttackTelegraph *telegraph =
            RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
      const Math::Vector3 start = RefGrandOwner().GetWorldPosition();
      const Math::Vector3 direction =
          (player->GetWorldPosition() - start).Normalized();
      telegraph->AddLine(start, ExtendRayWithinStageBounds(start, direction));
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

