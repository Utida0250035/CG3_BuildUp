#pragma once

#include "Math/Vector3.h"
#include "System/Entity/Entity.h"

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

/// 雑魚 1 体を生成して返す（採用は呼び出し側。y は体の半径に合わせる）。
/// index % variants.size() で種類が決まる。ボスの召喚からも使う。
std::unique_ptr<Entity> CreateAtomFEnemy(const Math::Vector3 &spawnPositionXZ,
                                         int index);

/// atomF シーンに敵をスポーンする。
void AppendAtomFEnemyIfScene(const std::string &sceneJsonPath,
                             std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum