#pragma once

#include "System/Entity/Entity.h"

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

/// atomF シーンにステージ進行役（ウェーブ・ボス登場）を置く。敵の生成はそこから行う。
void AppendAtomFEnemyIfScene(const std::string &sceneJsonPath,
                             std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum
