#include "System/CmpEnemy.h"
#include "System/BehEnemyMeteor.h"
#include "System/BehEnemyOrbiterSweep.h"
#include "System/BehEnemyShot.h"
#include "System/BehEnemyWallShot.h"
#include "System/CmpAttackTelegraph.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpEnemy::ResolveDependence() {
  behaviorBox_.SetGrandOwner(&RefOwner());
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void CmpEnemy::Initialize() {
  behaviorBox_.Finalize();
  behaviorBox_.InitializeTransitionCount();
  behaviorBox_.Transition(new BehEnemyShot());
  patternTransitionWarmupTimer_ = kPatternTransitionWarmupTime;
}

void CmpEnemy::Update() {
  if (CmpAttackTelegraph *telegraph =
          RefOwner().GetDataCmp<CmpAttackTelegraph>()) {
    telegraph->Clear();
  }

  behaviorBox_.Execute();

  if (patternTransitionWarmupTimer_ > 0.0f) {
    patternTransitionWarmupTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (patternTransitionWarmupTimer_ < 0.0f) {
      patternTransitionWarmupTimer_ = 0.0f;
    }
    return;
  }

  const uint64_t transitionCount = behaviorBox_.GetTransitionCount();
  if (transitionCount == 0) {
    return;
  }

  if ((transitionCount + 1) % behaviorTransitionCountForShotMeteor_ == 0) {
    behaviorBox_.Transition(new BehEnemyMeteor());
  } else if ((transitionCount + 1) % behaviorTransitionCountForOrbiterSweep_ ==
             0) {
    behaviorBox_.Transition(new BehEnemyOrbiterSweep());
  } else if ((transitionCount + 1) % behaviorTransitionCountForWallShot_ == 0) {
    behaviorBox_.Transition(new BehEnemyWallShot());
  }
}

} // namespace Atrum
