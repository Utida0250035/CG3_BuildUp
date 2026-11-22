#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

namespace EntityEditor {
class EntityEditor;
}

/// SceneEntityEditor プレビュー再構築が SceneBuildUp と同じ BuildEntities 経路であることを明示。
std::vector<std::unique_ptr<Entity>>
RebuildPreviewEntitiesViaBuildUpParity(EntityEditor::EntityEditor &editor);

} // namespace Atrum