#include "System/AtomF/AtomFScenePlayerLocator.h"

#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum::AtomF {

ScenePlayerLocator &ScenePlayerLocator::Instance() {
  static ScenePlayerLocator instance;
  return instance;
}

void ScenePlayerLocator::SetPlayerEntity(Entity *player) {
  player_ = player;
}

void RefreshScenePlayerLocatorFromEntities(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  Entity *found = nullptr;
  for (const std::unique_ptr<Entity> &entity : entities) {
    if (entity == nullptr || !entity->IsActive()) {
      continue;
    }
    if (tags.EntityHasRole(*entity, "player")) {
      found = entity.get();
      break;
    }
  }

  ScenePlayerLocator::Instance().SetPlayerEntity(found);
}

} // namespace Atrum::AtomF