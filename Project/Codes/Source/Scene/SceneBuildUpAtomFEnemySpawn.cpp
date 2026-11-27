#include "Scene/SceneBuildUpAtomFEnemySpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFEnemyHitAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackFx.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackPredictCapsule.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyShooter.h"
#include "System/AtomF/CmpAtomFEnemySpawnIn.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"
#include "System/AtomF/CmpDrawAtomFBossTelegraph.h"
#include "System/AtomF/CmpDrawAtomFEnemyKnockbackPredictCapsule.h"
#include "System/AtomF/CmpDrawAtomFEnemyTelegraph.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <string>
#include <vector>

namespace Atrum {

namespace {

/// 雑魚 1 体を組み立てる（Initialize 済み）。index は名前の一意化用。
std::unique_ptr<Entity> CreateAtomFEnemy(
    const AtomF::EnemyVariantParameters &variant, const float x, const float z,
    const int index) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float bodyRadius = AtomF::ResolveEnemyBodyRadiusMeters();

  std::unique_ptr<Entity> enemy =
      std::make_unique<Entity>("AtomFEnemy_" + std::to_string(index));
  enemy->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {x, bodyRadius, z};
  enemy->SetInitialTransform(initialTransform);

  enemy->AddUpdCmp<CmpAtomFEnemyVariant>();
  enemy->GetUpdCmp<CmpAtomFEnemyVariant>()->SetParameters(variant);

  enemy->AddUpdCmp<CmpAtomFEnemyKnockback>();
  enemy->AddUpdCmp<CmpAtomFEnemyChainExplosion>();
  enemy->AddUpdCmp<CmpAtomFEnemyTargetedPanic>();
  enemy->AddUpdCmp<CmpAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddDrawCmp<CmpDrawAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddUpdCmp<CmpAtomFEnemyWalkToPlayer>();
  enemy->AddUpdCmp<CmpAtomFEnemyPeriodicCharge>();
  enemy->AddUpdCmp<CmpAtomFEnemyShooter>();
  enemy->AddUpdCmp<CmpAtomFEnemySpawnIn>();
  enemy->AddDrawCmp<CmpDrawAtomFEnemyTelegraph>();

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

const AtomF::EnemyVariantParameters &FindVariant(const std::string &name) {
  static const AtomF::EnemyVariantParameters kDefault{};
  const AtomF::ParametersRuntime &runtime = AtomF::ParametersRuntime::Instance();
  if (!runtime.IsLoaded() || runtime.Enemy().variants.empty()) {
    return kDefault;
  }
  const std::vector<AtomF::EnemyVariantParameters> &variants =
      runtime.Enemy().variants;
  for (const AtomF::EnemyVariantParameters &variant : variants) {
    if (variant.name == name) {
      return variant;
    }
  }
  return variants.front();
}

} // namespace

void EnqueueAtomFEnemy(const std::string &variantName, const float x,
                       const float z) {
  static int sequence = 0;
  std::unique_ptr<Entity> enemy =
      CreateAtomFEnemy(FindVariant(variantName), x, z, sequence++);
  if (enemy == nullptr) {
    return;
  }
  Particle::Combat::PlayEnemySpawnPopEffect(
      Math::Vector3{x, AtomF::ResolveEnemyBodyRadiusMeters(), z});
  AtomF::EnqueueSceneEntityAdopt(std::move(enemy));
}

void EnqueueAtomFBoss(const float x, const float z) {
  if (CmpAtomFBoss::Active() != nullptr) {
    return;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const AtomF::BossParameters &params = AtomF::ResolveBossParameters();
  const float radius = params.bodyRadiusMeters;

  std::unique_ptr<Entity> boss = std::make_unique<Entity>("AtomFBoss");
  boss->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {x, radius, z};
  boss->SetInitialTransform(initialTransform);

  boss->AddUpdCmp<CmpAtomFBoss>();

  boss->AddDrawCmp<CmpMeshSphere>();
  if (CmpMeshSphere *mesh = boss->GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(radius);
    mesh->SetColor(
        Math::Vector4{params.colorR, params.colorG, params.colorB, 1.0f});
    mesh->SetSubdivision(24);
    mesh->SetLightingEnabled(false);
  }

  AtomF::AttachArenaContainment(*boss, radius);
  boss->AddUpdCmp<CmpHitSphere>();
  if (CmpHitSphere *hit = boss->GetUpdCmp<CmpHitSphere>()) {
    hit->SetSpheres({{{}, radius}});
  }
  boss->AddDrawCmp<CmpDrawAtomFBossTelegraph>();

  boss->ResolveDependencies();
  boss->Initialize();
  AtomF::EnqueueSceneEntityAdopt(std::move(boss));
}

} // namespace Atrum
