#include "Scene/SceneEntityEditorPreviewEditorAssetPathRelink.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneRuntimeEditorAssetPathPreviewRelink.h"

namespace Atrum {

void RelinkPreviewEditorAssetPathsAfterRebuild(
    EntityEditor::EntityEditor &editor,
    const std::vector<std::unique_ptr<Entity>> &previewEntities) {
  EntityEditor::RelinkEditorAssetPathPreviewsInScene(
      previewEntities, editor.SceneDocument(), nullptr);
}

} // namespace Atrum