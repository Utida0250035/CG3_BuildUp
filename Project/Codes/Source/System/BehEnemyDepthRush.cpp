#include "System/BehEnemyDepthRush.h"

#include "System/AttackTelegraph/AttackTelegraphUtils.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpEnemy.h"
#include "System/CmpOrbiter.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/OrbiterChildUtils.h"
#include "System/PlaySe.h"
#include "Math/Easing.h"
#include "Math/Lerp.h"

#include <algorithm>

namespace Atrum {

namespace {

using OrbiterChildUtils::ForEachBossOrbiter;

} // namespace

void BehEnemyDepthRush::ApplyBossWorldPosition(
    const Math::Vector3 &worldPosition) {
  RefGrandOwner().RefTransform().translate = worldPosition;
}

void BehEnemyDepthRush::PauseOrbitersAtDefaults() {
  ForEachBossOrbiter(RefGrandOwner(), [](CmpOrbiter &orbiter) {
    orbiter.CaptureDefaults();
    orbiter.RestoreDefaults();
  });
}

void BehEnemyDepthRush::RestoreOrbiters() {
  ForEachBossOrbiter(RefGrandOwner(), [](CmpOrbiter &orbiter) {
    orbiter.RestoreDefaults();
  });
}

void BehEnemyDepthRush::LockPlayerChargeWorld() {
  Entity *player = cmpEnemy_ != nullptr ? cmpEnemy_->GetPlayer() : nullptr;
  if (player == nullptr) {
    lockedPlayerChargeWorld_ = RefGrandOwner().GetWorldPosition();
    return;
  }

  lockedPlayerChargeWorld_ = player->GetWorldPosition();
}

void BehEnemyDepthRush::RegisterChargeTelegraph() {
  if (!RefGrandOwner().HasDataCmp<CmpAttackTelegraph>()) {
    return;
  }

  CmpAttackTelegraph *telegraph =
      RefGrandOwner().GetDataCmp<CmpAttackTelegraph>();
  AddBossMeleeTelegraphLine(telegraph, RefGrandOwner().GetWorldPosition(),
                            lockedPlayerChargeWorld_);
}

void BehEnemyDepthRush::RegisterReturnTelegraph() {
  if (!RefGrandOwner().HasDataCmp<CmpAttackTelegraph>()) {
    return;
  }

  CmpAttackTelegraph *telegraph =
      RefGrandOwner().GetDataCmp<CmpAttackTelegraph>();
  AddBossMeleeTelegraphLine(telegraph, RefGrandOwner().GetWorldPosition(),
                            battlePosition_);
}

void BehEnemyDepthRush::ResolveDependence() {
  cmpEnemy_ = RefGrandOwner().GetUpdCmp<CmpEnemy>();
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void BehEnemyDepthRush::Initialize() {
  phase_ = Phase::Telegraph;
  telegraphTimer_ = kTelegraphSeconds;
  retreatTimer_ = 0.0f;
  chargeTelegraphTimer_ = 0.0f;
  chargeTimer_ = 0.0f;
  returnTelegraphTimer_ = 0.0f;
  returnTimer_ = 0.0f;
  battlePosition_ = RefGrandOwner().GetWorldPosition();
  depthPosition_ = battlePosition_ + Math::Vector3{0.0f, 0.0f, kDepthBackOffsetZ};

  PauseOrbitersAtDefaults();

  if (CmpAttackTelegraph *telegraph =
          RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
    BeginBossMeleeTelegraphSession(telegraph);
  }
}

void BehEnemyDepthRush::Execute() {
  if (frameDeltaTime_ == nullptr || cmpEnemy_ == nullptr) {
    return;
  }

  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Telegraph) {
    if (CmpAttackTelegraph *telegraph =
            RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
      Entity *player = cmpEnemy_->GetPlayer();
      if (player != nullptr) {
        AddBossMeleeTelegraphLine(telegraph, RefGrandOwner().GetWorldPosition(),
                                  player->GetWorldPosition());
      }
    }

    telegraphTimer_ -= deltaTime;
    if (telegraphTimer_ > 0.0f) {
      return;
    }

    phase_ = Phase::RetreatToDepth;
    retreatTimer_ = 0.0f;
    return;
  }

  if (phase_ == Phase::RetreatToDepth) {
    retreatTimer_ += deltaTime;
    const float ratio =
        std::clamp(retreatTimer_ / kRetreatToDepthSeconds, 0.0f, 1.0f);
    const float progress = Math::Ease::InOutQuad(ratio);
    ApplyBossWorldPosition(Math::Lerp(battlePosition_, depthPosition_, progress));

    if (retreatTimer_ < kRetreatToDepthSeconds) {
      return;
    }

    ApplyBossWorldPosition(depthPosition_);
    LockPlayerChargeWorld();
    phase_ = Phase::ChargeTelegraph;
    chargeTelegraphTimer_ = 0.0f;
    if (CmpAttackTelegraph *telegraph =
            RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
      BeginBossMeleeTelegraphSession(telegraph);
    }
    return;
  }

  if (phase_ == Phase::ChargeTelegraph) {
    RegisterChargeTelegraph();

    chargeTelegraphTimer_ += deltaTime;
    if (chargeTelegraphTimer_ < kChargeTelegraphSeconds) {
      return;
    }

    chargeStartPosition_ = RefGrandOwner().GetWorldPosition();
    Math::Vector3 chargeTarget = lockedPlayerChargeWorld_;
    chargeTarget.z = battlePosition_.z;

    phase_ = Phase::ChargeToPlayer;
    chargeTimer_ = 0.0f;
    PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                      "./Resources/seEnemyShot.mp3"});
    return;
  }

  if (phase_ == Phase::ChargeToPlayer) {
    chargeTimer_ += deltaTime;
    const float ratio = std::clamp(chargeTimer_ / kChargeSeconds, 0.0f, 1.0f);
    const float progress = Math::Ease::InQuad(ratio);

    Math::Vector3 chargeTarget = lockedPlayerChargeWorld_;
    chargeTarget.z = battlePosition_.z;
    ApplyBossWorldPosition(
        Math::Lerp(chargeStartPosition_, chargeTarget, progress));

    if (chargeTimer_ < kChargeSeconds) {
      return;
    }

    ApplyBossWorldPosition(chargeTarget);
    phase_ = Phase::ReturnTelegraph;
    returnTelegraphTimer_ = 0.0f;
    if (CmpAttackTelegraph *telegraph =
            RefGrandOwner().GetDataCmp<CmpAttackTelegraph>()) {
      BeginBossMeleeTelegraphSession(telegraph);
    }
    return;
  }

  if (phase_ == Phase::ReturnTelegraph) {
    RegisterReturnTelegraph();

    returnTelegraphTimer_ += deltaTime;
    if (returnTelegraphTimer_ < kReturnTelegraphSeconds) {
      return;
    }

    returnMoveStartPosition_ = RefGrandOwner().GetWorldPosition();
    phase_ = Phase::ReturnToBattle;
    returnTimer_ = 0.0f;
    return;
  }

  if (phase_ == Phase::ReturnToBattle) {
    returnTimer_ += deltaTime;
    const float ratio =
        std::clamp(returnTimer_ / kReturnToBattleSeconds, 0.0f, 1.0f);
    const float progress = Math::Ease::InOutQuad(ratio);
    ApplyBossWorldPosition(
        Math::Lerp(returnMoveStartPosition_, battlePosition_, progress));

    if (returnTimer_ < kReturnToBattleSeconds) {
      return;
    }

    ApplyBossWorldPosition(battlePosition_);
    RestoreOrbiters();
    cmpEnemy_->TransitionNextShotBehavior();
  }
}

void BehEnemyDepthRush::Finalize() {
  ApplyBossWorldPosition(battlePosition_);
  RestoreOrbiters();
}

} // namespace Atrum
