#pragma once

#include <memory>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum {

void PurgeDestroyedSceneEntities(std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum