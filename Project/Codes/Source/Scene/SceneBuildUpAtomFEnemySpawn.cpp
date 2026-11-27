#include "Scene/SceneBuildUpAtomFEnemySpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFEnemyHitAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackFx.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackPredictCapsule.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"
#include "System/AtomF/CmpDrawAtomFEnemyKnockbackPredictCapsule.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <cmath>

namespace Atrum {

namespace {

int NextSpawnIndex() {
  static int counter = 0;
  return counter++;
}

/// 雑魚 1 体を組み立てて返す（ResolveDependencies / Initialize 済み）。
std::unique_ptr<Entity> CreateAtomFEnemyEntity(const int variantIndex,
                                               const float worldX,
                                               const float worldZ) {
  const int index = NextSpawnIndex();
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float bodyRadius = AtomF::ResolveEnemyBodyRadiusMeters();

  std::unique_ptr<Entity> enemy =
      std::make_unique<Entity>("AtomFEnemy_" + std::to_string(index));
  enemy->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {worldX, bodyRadius, worldZ};
  enemy->SetInitialTransform(initialTransform);

  // 種類（variants）を index % 件数 で割り当てる。未定義なら従来の単一種。
  AtomF::EnemyVariantParameters variant{};
  if (AtomF::ParametersRuntime::Instance().IsLoaded()) {
    const auto &variants = AtomF::ParametersRuntime::Instance().Enemy().variants;
    if (!variants.empty()) {
      const int count = static_cast<int>(variants.size());
      const int wrapped = ((variantIndex % count) + count) % count;
      variant = variants[static_cast<std::size_t>(wrapped)];
    }
  }
  enemy->AddUpdCmp<CmpAtomFEnemyVariant>();
  enemy->GetUpdCmp<CmpAtomFEnemyVariant>()->SetParameters(variant);

  enemy->AddUpdCmp<CmpAtomFEnemyKnockback>();
  enemy->AddUpdCmp<CmpAtomFEnemyChainExplosion>();
  enemy->AddUpdCmp<CmpAtomFEnemyTargetedPanic>();
  enemy->AddUpdCmp<CmpAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddDrawCmp<CmpDrawAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddUpdCmp<CmpAtomFEnemyWalkToPlayer>();
  enemy->AddUpdCmp<CmpAtomFEnemyPeriodicCharge>();

  enemy->AddDrawCmp<CmpMeshSphere>();
  CmpMeshSphere *mesh = enemy->GetDrawCmp<CmpMeshSphere>();
  if (mesh != nullptr) {
    mesh->SetRadius(bodyRadius);
    mesh->SetColor(Math::Vector4{variant.colorR, variant.colorG, variant.colorB, 1.0f});
    mesh->SetSubdivision(10);
    mesh->SetLightingEnabled(false);
    AtomF::AttachMeshModel(*mesh, AtomF::ResolveEnemyModelName());
  }

  AtomF::AttachArenaContainmentForEnemy(*enemy);
  AtomF::AttachEnemyHitSphere(*enemy);

  // 吹き飛び演出: 子エンティティにエミッタを付け、敵の行列を親として継承する。
  const std::string enemyName = "AtomFEnemy_" + std::to_string(index);
  std::unique_ptr<Entity> trailChild =
      std::make_unique<Entity>(enemyName + "_KnockbackTrail");
  trailChild->AddUpdCmp<CmpParticleEmitter>();
  CmpParticleEmitter *trailEmitter = trailChild->GetUpdCmp<CmpParticleEmitter>();
  trailEmitter->SetJsonFilePath("atomf_knockback_trail.json");
  trailEmitter->SetInheritOwnerMatrix(true);
  trailEmitter->SetEmitEnabled(false);
  // 発生範囲の幅を敵の直径と同程度にする（円の半径 = 敵の半径）。
  trailEmitter->SetEmitCircleRadiusOverride(bodyRadius);

  std::unique_ptr<Entity> auraChild =
      std::make_unique<Entity>(enemyName + "_KnockbackAura");
  auraChild->AddUpdCmp<CmpParticleEmitter>();
  CmpParticleEmitter *auraEmitter = auraChild->GetUpdCmp<CmpParticleEmitter>();
  auraEmitter->SetJsonFilePath("atomf_knockback_aura.json");
  auraEmitter->SetInheritOwnerMatrix(true);
  auraEmitter->SetEmitEnabled(false);

  enemy->AddUpdCmp<CmpChildEntitys>();
  enemy->GetUpdCmp<CmpChildEntitys>()->AddChilds(
      {trailChild.release(), auraChild.release()});
  enemy->AddUpdCmp<CmpAtomFEnemyKnockbackFx>();
  CmpAtomFEnemyKnockbackFx *knockbackFx =
      enemy->GetUpdCmp<CmpAtomFEnemyKnockbackFx>();
  knockbackFx->SetTrailEmitter(trailEmitter);
  knockbackFx->SetAuraEmitter(auraEmitter);

  enemy->ResolveDependencies();
  enemy->Initialize();
  return enemy;
}

/// ボス 1 体を組み立てて返す。Hp は CmpHealthHit、行動は CmpAtomFBoss が持つ。
std::unique_ptr<Entity> CreateAtomFBossEntity(const float worldX,
                                              const float worldZ) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const AtomF::BossParameters &params = AtomF::ResolveBossParameters();
  const float radius = params.bodyRadiusMeters;

  std::unique_ptr<Entity> boss = std::make_unique<Entity>("AtomFBoss");
  // 自機の接触ダメージ・敵同士の衝突爆発・予測判定の対象にするため enemy 役を付ける。
  boss->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  // 出現演出で地面下から湧き上がるため、初期 Y は沈めておく。
  initialTransform.translate = {worldX, -radius * 1.2f, worldZ};
  boss->SetInitialTransform(initialTransform);

  boss->AddUpdCmp<CmpHitSphere>();
  if (CmpHitSphere *hit = boss->GetUpdCmp<CmpHitSphere>()) {
    hit->SetSpheres({{{}, radius}});
  }

  boss->AddUpdCmp<CmpHealthHit>();
  if (CmpHealthHit *health = boss->GetUpdCmp<CmpHealthHit>()) {
    // 被弾の判定は CmpAtomFBoss::TakeDamage が行うため、攻撃元タグは登録しない。
    health->SetInitialHealthCount(static_cast<uint32_t>(params.maxHealth));
    health->SetHealthCount(static_cast<uint32_t>(params.maxHealth));
    health->SetInvincibleTime(0.0f);
  }

  boss->AddUpdCmp<CmpAtomFBoss>();

  boss->AddDrawCmp<CmpMeshSphere>();
  if (CmpMeshSphere *mesh = boss->GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(radius * 0.4f);
    mesh->SetColor(Math::Vector4{params.colorR, params.colorG, params.colorB,
                                 1.0f});
    mesh->SetSubdivision(20);
    mesh->SetLightingEnabled(false);
  }

  AtomF::AttachArenaContainment(*boss, radius);

  boss->ResolveDependencies();
  boss->Initialize();
  return boss;
}

} // namespace

namespace AtomF {

int EnemyVariantCount() {
  if (!ParametersRuntime::Instance().IsLoaded()) {
    return 1;
  }
  const int count =
      static_cast<int>(ParametersRuntime::Instance().Enemy().variants.size());
  return count > 0 ? count : 1;
}

int FindEnemyVariantIndex(const std::string &name) {
  if (!ParametersRuntime::Instance().IsLoaded()) {
    return -1;
  }
  const auto &variants = ParametersRuntime::Instance().Enemy().variants;
  for (std::size_t i = 0; i < variants.size(); ++i) {
    if (variants[i].name == name) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

bool SpawnEnemyAtWorld(const int variantIndex, const float worldX,
                       const float worldZ) {
  std::unique_ptr<Entity> enemy =
      CreateAtomFEnemyEntity(variantIndex, worldX, worldZ);
  if (enemy == nullptr) {
    return false;
  }
  // 出現の合図（小さな煙）。
  Particle::Combat::PlayOneShotParticleFile(
      "./Assets/Particles/enemy_spawn.json",
      Math::Vector3{worldX, 0.1f, worldZ});
  EnqueueSceneEntityAdopt(std::move(enemy));
  return true;
}

bool SpawnBossAtWorld(const float worldX, const float worldZ) {
  std::unique_ptr<Entity> boss = CreateAtomFBossEntity(worldX, worldZ);
  if (boss == nullptr) {
    return false;
  }
  EnqueueSceneEntityAdopt(std::move(boss));
  return true;
}

} // namespace AtomF

void AppendAtomFEnemyIfScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  (void)entities;
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    return;
  }
  // 敵・ボスの出現はステージスクリプト（Assets/Scripts/atomF/stage.lua）が
  // AtomF.spawnEnemy / AtomF.spawnBoss で行う。入場時には何も置かない。
}

} // namespace Atrum
