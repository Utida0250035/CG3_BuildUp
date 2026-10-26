#include "System/BossCombatEnrage.h"
#include "System/BossFacingUtils.h"
#include "System/Entity/Components/CmpEnemy.h"
#include "System/BehEnemyBurstShot.h"
#include "System/BehEnemyMeteor.h"
#include "System/BehEnemyDepthRush.h"
#include "System/BehEnemyShot.h"
#include "System/BehEnemySpreadShot.h"
#include "System/BehEnemyWallShot.h"
#include "System/AttackTelegraph/CmpAttackTelegraph.h"
#include "System/Entity/Components/CmpDrawHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

namespace {

/// 特殊行動のローテーション周期（大きいほど弾幕系の間隔が空く）
constexpr uint64_t kSpecialPatternCycle = 28;

constexpr Math::Vector4 kBossDefaultShellColor{0.75f, 0.25f, 0.125f, 0.45f};
constexpr Math::Vector4 kBossDefaultInnerColor{0.75f, 0.25f, 0.125f, 1.0f};

void ApplyBossBodyColorFromHealth(Entity &boss) {
  CmpHealthHit *health = boss.GetUpdCmp<CmpHealthHit>();
  CmpMeshSphere *mesh = boss.GetDrawCmp<CmpMeshSphere>();
  CmpDrawHitSphere *innerSphere = boss.GetDrawCmp<CmpDrawHitSphere>();
  if (health == nullptr || mesh == nullptr) {
    return;
  }

  if (health->IsHurtFlashActive()) {
    mesh->SetColor(health->GetHurtFlashColor());
    if (innerSphere != nullptr) {
      innerSphere->SetColor(health->GetHurtFlashInnerColor());
    }
  } else {
    mesh->SetColor(kBossDefaultShellColor);
    if (innerSphere != nullptr) {
      innerSphere->SetColor(kBossDefaultInnerColor);
    }
  }
}

} // namespace

void CmpEnemy::ResolveDependence() {
  behaviorBox_.SetGrandOwner(&RefOwner());
  frameDeltaTime_ = FrameDeltaTime::Instance();
}

void CmpEnemy::Initialize() {
  behaviorBox_.Finalize();
  behaviorBox_.InitializeTransitionCount();
  shotPatternIndex_ = 0;
  TransitionNextShotBehavior();
  patternTransitionWarmupTimer_ = kPatternTransitionWarmupTime;
}

void CmpEnemy::TransitionNextShotBehavior() {
  // 通常ローテ：狙い撃ち＋突進を多め、拡散・連射は控えめ
  switch (shotPatternIndex_ % 5) {
  case 0:
  case 1:
  case 2:
    behaviorBox_.Transition(new BehEnemyShot());
    break;
  case 3:
    behaviorBox_.Transition(new BehEnemySpreadShot());
    break;
  case 4:
    behaviorBox_.Transition(new BehEnemyBurstShot());
    break;
  }
  ++shotPatternIndex_;
}

void CmpEnemy::Update() {
  if (CmpAttackTelegraph *telegraph =
          RefOwner().GetDataCmp<CmpAttackTelegraph>()) {
    telegraph->Clear();
  }

  ApplyBossBodyColorFromHealth(RefOwner());
  BossCombatEnrage::SyncOrbiterRotateSpeeds(RefOwner());

  if (Entity *player = player_) {
    ApplyBossFacingTowardPlayer(RefOwner(), *player);
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

  switch ((transitionCount + 1) % kSpecialPatternCycle) {
  case 7:
  case 14:
  case 21:
    behaviorBox_.Transition(new BehEnemyDepthRush());
    break;
  case 24:
    behaviorBox_.Transition(new BehEnemyMeteor());
    break;
  case 26:
    behaviorBox_.Transition(new BehEnemyWallShot());
    break;
  // 放射／固定リング弾幕は頻度を下げるためローテから外す
  default:
    break;
  }
}

} // namespace Atrum
