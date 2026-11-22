#include "Scene/SceneEntityEditorPreviewBuildUpParityRebuild.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum {

std::vector<std::unique_ptr<Entity>>
RebuildPreviewEntitiesViaBuildUpParity(EntityEditor::EntityEditor &editor) {
  return editor.BuildPreviewScene();
}

} // namespace Atrum