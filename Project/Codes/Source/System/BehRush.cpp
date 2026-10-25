#include "System/BehRush.h"
#include "Math/Easing.h"
#include "Math/Vector3.h"
#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpEnemy.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void BehRush::ResolveDependence() {
  frameDeltaTime_ = FrameDeltaTime::Instance();
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
}

void BehRush::Initialize() {
  sourcePos_ = RefGrandOwner().GetWorldPosition();
  timer_ = 0.0f;
  meleeTelegraphSessionStarted_ = false;
}

void BehRush::Execute() {
  if (CmpAttackTelegraph *telegraph =
          RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
    if (!meleeTelegraphSessionStarted_) {
      BeginBossMeleeTelegraphSession(telegraph);
      meleeTelegraphSessionStarted_ = true;
    }

    AddBossMeleeTelegraphLine(telegraph, RefGrandOwner().GetWorldPosition(),
                              destinationPos_);
  }

  timer_ += frameDeltaTime_->GetDeltaTime();
  if (timer_ > endTime_) {
    timer_ = endTime_;
  }

  RefGrandOwner().RefTransform().translate =
      Math::Lerp(sourcePos_, destinationPos_, Math::Ease::InBack(timer_ / endTime_));

  if (timer_ >= endTime_) {
    cmpEnemy_->TransitionNextShotBehavior();
  }
}

} // namespace Atrum
