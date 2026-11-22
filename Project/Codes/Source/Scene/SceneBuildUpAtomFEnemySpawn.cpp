#include "Scene/SceneBuildUpAtomFEnemySpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFEnemyHitAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum {

void AppendAtomFEnemyIfScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    return;
  }

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float bodyRadius = AtomF::ResolveEnemyBodyRadiusMeters();

  std::unique_ptr<Entity> enemy = std::make_unique<Entity>("AtomFEnemy");
  enemy->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {8.0f, bodyRadius, 0.0f};
  enemy->SetInitialTransform(initialTransform);

  enemy->AddUpdCmp<CmpAtomFEnemyKnockback>();
  enemy->AddUpdCmp<CmpAtomFEnemyWalkToPlayer>();
  enemy->AddUpdCmp<CmpAtomFEnemyPeriodicCharge>();
  enemy->AddUpdCmp<CmpAtomFEnemyTargetedPanic>();

  enemy->AddDrawCmp<CmpMeshSphere>();
  CmpMeshSphere *mesh = enemy->GetDrawCmp<CmpMeshSphere>();
  if (mesh != nullptr) {
    mesh->SetRadius(bodyRadius);
    mesh->SetColor(Math::Vector4{0.92f, 0.28f, 0.25f, 1.0f});
    mesh->SetSubdivision(10);
  }

  AtomF::AttachArenaContainmentForEnemy(*enemy);
  AtomF::AttachEnemyHitSphere(*enemy);

  enemy->ResolveDependencies();
  enemy->Initialize();
  entities.push_back(std::move(enemy));
}

} // namespace Atrum