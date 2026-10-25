#include "System/BossCombatEnrage.h"
#include "System/BossFacingUtils.h"
#include "System/CmpEnemy.h"
#include "System/BehEnemyBurstShot.h"
#include "System/BehEnemyBulletCurtainFixedRing.h"
#include "System/BehEnemyBulletCurtainRadialFan.h"
#include "System/BehEnemyMeteor.h"
#include "System/BehEnemyDepthRush.h"
#include "System/BehEnemyShot.h"
#include "System/BehEnemySpreadShot.h"
#include "System/BehEnemyWallShot.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpDrawHitSphere.h"
#include "System/CmpMeshSphere.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

namespace {

/// 特殊行動（メテオ・壁弾・突進・オービター・弾幕）のローテーション周期
constexpr uint64_t kSpecialPatternCycle = 20;

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
  switch (shotPatternIndex_ % 3) {
  case 0:
    behaviorBox_.Transition(new BehEnemyShot());
    break;
  case 1:
    behaviorBox_.Transition(new BehEnemySpreadShot());
    break;
  case 2:
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
  case 0:
    behaviorBox_.Transition(new BehEnemyMeteor());
    break;
  case 4:
    behaviorBox_.Transition(new BehEnemyWallShot());
    break;
  case 8:
    behaviorBox_.Transition(new BehEnemyDepthRush());
    break;
  // 11 / 14: オービター弾（3D 方向）は奥行きが分かりにくいため一旦未使用
  case 17:
    behaviorBox_.Transition(new BehEnemyBulletCurtainRadialFan());
    break;
  case 19:
    behaviorBox_.Transition(new BehEnemyBulletCurtainFixedRing());
    break;
  default:
    break;
  }
}

} // namespace Atrum
