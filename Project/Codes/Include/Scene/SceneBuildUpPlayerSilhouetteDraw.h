#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class AtrumEngine;
class Entity;

/// SceneBuildUp のランタイム描画末尾で地形越し自機シルエットを出す
namespace SceneBuildUpPlayerSilhouette {

void DrawAfterRuntimeEntities(
    AtrumEngine *engine,
    const std::vector<std::unique_ptr<Entity>> &entities);

} // namespace SceneBuildUpPlayerSilhouette

} // namespace Atrum