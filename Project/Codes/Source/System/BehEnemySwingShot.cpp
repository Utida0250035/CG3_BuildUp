#include "System/BehEnemySwingShot.h"
#include "System/BehRush.h"
#include "System/Entity/Components/CmpEnemy.h"
#include "System/Entity/Components/CmpReflectAbleBullet.h"
#include "System/Entity/Components/CmpRotator.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/PlaySe.h"

namespace Atrum {

void BehEnemySwingShot::ResolveDependence() {
  cmpBullet_ = RefGrandOwner().GetUpdCmp<CmpReflectAbleBullet>();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  cmpRotator_ = RefGrandOwner().GetUpdCmp<CmpRotator>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemySwingShot::Initialize() { shotCoolTimer_ = 0.0f; }

void BehEnemySwingShot::Execute() {
  if (shotCoolTimer_ <= 0.0f) {
    shotCoolTimer_ = shotCoolTime_;
    std::unique_ptr<Entity> entBullet = EntityFactory::LinearEnemyBullet({rapidBulletSpeed_, 0.0f, 0.0f}, {});
    entBullet->SetParent(&cmpRotator_->RefPivot());
    cmpBullet_->Shot(entBullet.release());
    PlayEnemyShotSe();
    ++shotCount_;
  } else {
    shotCoolTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (shotCoolTimer_ < 0.0f) {
      shotCoolTimer_ = 0.0f;
    }
  }

  if (shotCount_ < shotCountMax_) {
    return;
  }

  shotCount_ = shotCountMax_;
  std::unique_ptr<BehRush> behRush = std::make_unique<BehRush>();
  behRush->SetDestinationPos(cmpEnemy_->GetPlayer()->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush.release());
}

} // namespace Atrum
