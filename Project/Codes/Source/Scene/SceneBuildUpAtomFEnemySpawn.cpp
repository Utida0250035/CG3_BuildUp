#include "Scene/SceneBuildUpAtomFEnemySpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFEnemyHitAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackFx.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackPredictCapsule.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"
#include "System/AtomF/CmpAtomFStageDirector.h"
#include "System/AtomF/CmpDrawAtomFEnemyKnockbackPredictCapsule.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr int kAtomFEnemySpawnCount = 6;

} // namespace

std::unique_ptr<Entity> CreateAtomFEnemy(const Math::Vector3 &spawnPositionXZ,
                                         const int index) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float bodyRadius = AtomF::ResolveEnemyBodyRadiusMeters();

  std::unique_ptr<Entity> enemy =
      std::make_unique<Entity>("AtomFEnemy_" + std::to_string(index));
  enemy->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {spawnPositionXZ.x, bodyRadius,
                                spawnPositionXZ.z};
  enemy->SetInitialTransform(initialTransform);

  // 種類（variants）を index % 件数 で割り当てる。未定義なら従来の単一種。
  AtomF::EnemyVariantParameters variant{};
  if (AtomF::ParametersRuntime::Instance().IsLoaded()) {
    const auto &variants = AtomF::ParametersRuntime::Instance().Enemy().variants;
    if (!variants.empty()) {
      variant = variants[static_cast<std::size_t>(index) % variants.size()];
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

void AppendAtomFEnemyIfScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    return;
  }

  // 円周上に複数配置し、連鎖爆発が起きやすい配置にする。
  constexpr float kSpawnRadiusMeters = 8.0f;
  constexpr float kTwoPi = 6.28318530718f;
  for (int i = 0; i < kAtomFEnemySpawnCount; ++i) {
    const float angle =
        (kTwoPi * static_cast<float>(i)) / static_cast<float>(kAtomFEnemySpawnCount);
    entities.push_back(CreateAtomFEnemy(
        Math::Vector3{std::cos(angle) * kSpawnRadiusMeters, 0.0f,
                      std::sin(angle) * kSpawnRadiusMeters},
        i));
  }

  // 雑魚が全滅したらボスを出す進行役。
  std::unique_ptr<Entity> director =
      std::make_unique<Entity>("AtomFStageDirector");
  director->AddUpdCmp<CmpAtomFStageDirector>();
  director->ResolveDependencies();
  director->Initialize();
  entities.push_back(std::move(director));
}

} // namespace Atrum
