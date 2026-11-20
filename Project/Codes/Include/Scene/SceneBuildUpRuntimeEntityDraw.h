#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

/// BuildUp 用のシーン Entity 描画（親子契約は EditorSceneRuntime と同一）
namespace SceneBuildUpRuntimeEntityDraw {

void Draw(const std::vector<std::unique_ptr<Entity>> &entities);

} // namespace SceneBuildUpRuntimeEntityDraw

} // namespace Atrum