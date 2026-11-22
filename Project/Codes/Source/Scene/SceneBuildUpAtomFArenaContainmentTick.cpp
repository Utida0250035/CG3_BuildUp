#include "Scene/SceneBuildUpAtomFArenaContainmentTick.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFArenaContainment.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum {

namespace {

float ContainmentBodyRadiusForEntity(const Entity &entity) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  if (tags.EntityHasRole(entity, "player")) {
    return AtomF::kPlayerBodyRadiusMeters;
  }
  if (tags.EntityHasRole(entity, "enemy")) {
    return AtomF::kEnemyBodyRadiusMeters;
  }
  if (tags.EntityHasManagedBulletRole(entity)) {
    return AtomF::kPlayerBulletBodyRadiusMeters;
  }

  const std::string playerWeaponTag = tags.TagNameForRole("playerWeapon");
  if (!playerWeaponTag.empty() &&
      tags.EntityHasTagName(entity, playerWeaponTag)) {
    return AtomF::kPlayerBulletBodyRadiusMeters;
  }

  return -1.0f;
}

} // namespace

void TickAtomFArenaContainmentForScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    return;
  }

  for (std::unique_ptr<Entity> &entity : entities) {
    if (entity == nullptr || !entity->IsActive()) {
      continue;
    }

    if (entity->HasUpdCmp<CmpAtomFArenaContainment>()) {
      continue;
    }

    const float bodyRadius = ContainmentBodyRadiusForEntity(*entity);
    if (bodyRadius < 0.0f) {
      continue;
    }

    AtomF::ClampWorldTranslateToArenaDisc(entity->RefTransform().translate,
                                          bodyRadius);
  }
}

} // namespace Atrum