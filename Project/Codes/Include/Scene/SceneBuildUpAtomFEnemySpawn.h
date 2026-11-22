#pragma once

#include "System/Entity/Entity.h"

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

/// atomF シーンに敵をスポーンする。
void AppendAtomFEnemyIfScene(const std::string &sceneJsonPath,
                             std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum