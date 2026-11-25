#include "Scene/SceneBuildUpAtomFEnemySpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFEnemyHitAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackPredictCapsule.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"
#include "System/AtomF/CmpDrawAtomFEnemyKnockbackPredictCapsule.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr int kAtomFEnemySpawnCount = 6;

void SpawnOneAtomFEnemy(std::vector<std::unique_ptr<Entity>> &entities,
                        const float spawnAngleRadians,
                        const float spawnRadiusMeters,
                        const int index) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float bodyRadius = AtomF::ResolveEnemyBodyRadiusMeters();

  std::unique_ptr<Entity> enemy =
      std::make_unique<Entity>("AtomFEnemy_" + std::to_string(index));
  enemy->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {std::cos(spawnAngleRadians) * spawnRadiusMeters,
                                bodyRadius,
                                std::sin(spawnAngleRadians) * spawnRadiusMeters};
  enemy->SetInitialTransform(initialTransform);

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
    mesh->SetColor(Math::Vector4{0.92f, 0.28f, 0.25f, 1.0f});
    mesh->SetSubdivision(10);
    mesh->SetLightingEnabled(false);
  }

  AtomF::AttachArenaContainmentForEnemy(*enemy);
  AtomF::AttachEnemyHitSphere(*enemy);

  enemy->ResolveDependencies();
  enemy->Initialize();
  entities.push_back(std::move(enemy));
}

} // namespace

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
    SpawnOneAtomFEnemy(entities, angle, kSpawnRadiusMeters, i);
  }
}

} // namespace Atrum
