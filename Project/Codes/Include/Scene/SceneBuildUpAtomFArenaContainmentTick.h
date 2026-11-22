#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

/// atomF シーンのみ。player / enemy / 管理弾 / playerWeapon 役のエンティティをアリーナ内に押し戻す。
void TickAtomFArenaContainmentForScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum