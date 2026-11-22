#include "Scene/SceneBuildUpAtomFEntityDestroyPurge.h"

#include "System/Entity/Entity.h"

#include <algorithm>
#include <memory>

namespace Atrum {

void PurgeDestroyedSceneEntities(
    std::vector<std::unique_ptr<Entity>> &entities) {
  std::erase_if(entities, [](const std::unique_ptr<Entity> &entity) {
    return entity != nullptr &&
           entity->GetState() == Entity::State::Destroy;
  });
}

} // namespace Atrum