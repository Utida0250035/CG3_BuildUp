#include "System/BehEnemyOrbiterSweep.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/BehRush.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpEnemy.h"
#include "System/CmpOrbiter.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/OrbiterChildUtils.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"
#include "Math/Easing.h"

#include <algorithm>

namespace Atrum {

namespace {

using OrbiterChildUtils::ForEachBossOrbiter;
using OrbiterChildUtils::OrbiterIndexToStartAngleRadians;

void RegisterGroundSweepTelegraph(CmpAttackTelegraph *telegraph) {
  RegisterFullWidthGroundLines(
      telegraph, kGroundHeight + 0.2f, 0.35f,
      AttackTelegraphPalette::kNonReflectAbleMain,
      AttackTelegraphPalette::kNonReflectAbleSub);
}

void RegisterOrbiterGatherTelegraphIfPresent(Entity &boss) {
  if (!boss.HasDataCmp<CmpAttackTelegraph>()) {
    return;
  }

  RegisterBossOrbiterGatherTelegraph(
      boss, boss.GetDataCmp<CmpAttackTelegraph>(),
      AttackTelegraphPalette::kNonReflectAbleMain);
}

} // namespace

void BehEnemyOrbiterSweep::ResolveDependence() {
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyOrbiterSweep::Initialize() {
  phase_ = Phase::Telegraph;
  telegraphTimer_ = kTelegraphSeconds;
  moveToStartTimer_ = 0.0f;
  spinUnderBossTimer_ = 0.0f;
  sweepTimer_ = 0.0f;
  holdBeforeGatherTimer_ = 0.0f;
  gatherOrbitersTimer_ = 0.0f;

  ForEachBossOrbiter(RefGrandOwner(), [](CmpOrbiter &orbiter) {
    orbiter.CaptureDefaults();
  });
}

void BehEnemyOrbiterSweep::BeginMoveToSweepStartPhase() {
  phase_ = Phase::MoveToSweepStart;
  moveToStartTimer_ = 0.0f;

  const float bossY = RefGrandOwner().GetTransform().translate.y;
  localOrbitHeightY_ = kGroundHeight - bossY + 0.25f;

  ForEachBossOrbiter(
      RefGrandOwner(),
      [&](CmpOrbiter &orbiter, const size_t index, const size_t orbiterCount) {
        orbiter.BeginMoveToSweepStart(
            localOrbitHeightY_, kSpinUnderBossHubRadius,
            OrbiterIndexToStartAngleRadians(index, orbiterCount));
      });
}

void BehEnemyOrbiterSweep::UpdateMoveToSweepStart() {
  const float ratio =
      std::clamp(moveToStartTimer_ / kMoveToSweepStartSeconds, 0.0f, 1.0f);
  const float progress = Math::Ease::InOutQuad(ratio);

  ForEachBossOrbiter(RefGrandOwner(), [progress](CmpOrbiter &orbiter) {
    orbiter.SetMoveToSweepStartProgress(progress);
  });
}

void BehEnemyOrbiterSweep::BeginSpinUnderBossPhase() {
  phase_ = Phase::SpinUnderBoss;
  spinUnderBossTimer_ = 0.0f;

  ForEachBossOrbiter(
      RefGrandOwner(),
      [&](CmpOrbiter &orbiter, const size_t index, const size_t orbiterCount) {
        orbiter.EndMoveToSweepStart();
        orbiter.BeginSpinUnderBoss(
            kSpinUnderBossRotateSpeed, localOrbitHeightY_,
            OrbiterIndexToStartAngleRadians(index, orbiterCount),
            kSpinUnderBossHubRadius);
      });
}

void BehEnemyOrbiterSweep::BeginSweepPhase() {
  phase_ = Phase::Sweep;
  sweepTimer_ = 0.0f;

  ForEachBossOrbiter(RefGrandOwner(), [](CmpOrbiter &orbiter) {
    orbiter.BeginGroundSweepExpandFromSpin(kSweepRotateSpeed, kSweepOrbitRadius);
  });

  PlaySe("./Resources/seEnemyShot.mp3");
}

void BehEnemyOrbiterSweep::UpdateSweepRadiusScale() {
  const float ratio =
      std::clamp(sweepTimer_ / kSweepSeconds, 0.0f, 1.0f);
  const float radiusScale = Math::Ease::InQuad(ratio);

  ForEachBossOrbiter(RefGrandOwner(), [radiusScale](CmpOrbiter &orbiter) {
    orbiter.SetGroundSweepRadiusScale(radiusScale);
  });
}

void BehEnemyOrbiterSweep::BeginHoldBeforeGatherPhase() {
  phase_ = Phase::HoldBeforeGather;
  holdBeforeGatherTimer_ = 0.0f;

  ForEachBossOrbiter(RefGrandOwner(), [](CmpOrbiter &orbiter) {
    orbiter.SetGroundSweepRadiusScale(1.0f);
    orbiter.BeginGatherTrajectoryHold();
  });
}

void BehEnemyOrbiterSweep::BeginGatherOrbitersPhase() {
  phase_ = Phase::GatherOrbiters;
  gatherOrbitersTimer_ = 0.0f;

  ForEachBossOrbiter(RefGrandOwner(), [](CmpOrbiter &orbiter) {
    orbiter.EndGatherTrajectoryHold();
    orbiter.BeginGatherToDefaultPosition();
  });
}

void BehEnemyOrbiterSweep::UpdateGatherOrbiters() {
  const float ratio =
      std::clamp(gatherOrbitersTimer_ / kGatherOrbitersSeconds, 0.0f, 1.0f);
  const float progress = Math::Ease::InOutQuad(ratio);

  ForEachBossOrbiter(RefGrandOwner(), [progress](CmpOrbiter &orbiter) {
    orbiter.SetGatherToDefaultProgress(progress);
  });
}

void BehEnemyOrbiterSweep::RestoreOrbiters() {
  ForEachBossOrbiter(RefGrandOwner(), [](CmpOrbiter &orbiter) {
    orbiter.EndGatherToDefaultPosition();
    orbiter.RestoreDefaults();
  });
}

void BehEnemyOrbiterSweep::TransitionToRush() {
  if (cmpEnemy_ == nullptr) {
    isFinished_ = true;
    return;
  }

  Entity *player = cmpEnemy_->GetPlayer();
  if (player == nullptr) {
    isFinished_ = true;
    return;
  }

  BehRush *behRush = new BehRush();
  behRush->SetDestinationPos(player->GetWorldPosition());
  cmpEnemy_->RefBehaviorBox().Transition(behRush);
}

void BehEnemyOrbiterSweep::Execute() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Telegraph) {
    if (RefGrandOwner().HasDataCmp<CmpAttackTelegraph>()) {
      RegisterGroundSweepTelegraph(
          RefGrandOwner().GetDataCmp<CmpAttackTelegraph>());
    }

    telegraphTimer_ -= deltaTime;
    if (telegraphTimer_ > 0.0f) {
      return;
    }

    BeginMoveToSweepStartPhase();
    return;
  }

  if (phase_ == Phase::MoveToSweepStart) {
    moveToStartTimer_ += deltaTime;
    UpdateMoveToSweepStart();

    if (moveToStartTimer_ < kMoveToSweepStartSeconds) {
      return;
    }

    BeginSpinUnderBossPhase();
    return;
  }

  if (phase_ == Phase::SpinUnderBoss) {
    spinUnderBossTimer_ += deltaTime;
    if (spinUnderBossTimer_ < kSpinUnderBossSeconds) {
      return;
    }

    BeginSweepPhase();
    return;
  }

  if (phase_ == Phase::Sweep) {
    sweepTimer_ += deltaTime;
    UpdateSweepRadiusScale();

    if (sweepTimer_ < kSweepSeconds) {
      return;
    }

    BeginHoldBeforeGatherPhase();
    return;
  }

  if (phase_ == Phase::HoldBeforeGather) {
    RegisterOrbiterGatherTelegraphIfPresent(RefGrandOwner());

    holdBeforeGatherTimer_ += deltaTime;
    if (holdBeforeGatherTimer_ < kHoldBeforeGatherSeconds) {
      return;
    }

    BeginGatherOrbitersPhase();
    return;
  }

  if (phase_ == Phase::GatherOrbiters) {
    RegisterOrbiterGatherTelegraphIfPresent(RefGrandOwner());

    gatherOrbitersTimer_ += deltaTime;
    UpdateGatherOrbiters();

    if (gatherOrbitersTimer_ < kGatherOrbitersSeconds) {
      return;
    }

    RestoreOrbiters();
    TransitionToRush();
  }
}

void BehEnemyOrbiterSweep::Finalize() { RestoreOrbiters(); }

} // namespace Atrum
