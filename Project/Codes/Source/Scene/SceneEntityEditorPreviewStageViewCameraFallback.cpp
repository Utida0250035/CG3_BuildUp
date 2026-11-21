#include "Scene/SceneEntityEditorPreviewStageViewCameraFallback.h"

#include "Scene/SceneEntityEditor.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"

namespace Atrum {

void ApplyPreviewStageViewCameraFallback(
    SceneEntityEditor &scene, const EntityEditor::EntityEditor &editor) {
  scene.SetStageViewFramingBounds(
      EntityEditor::ResolveStageViewFramingBounds(editor.SceneDocument()));
  scene.SetupStageViewCamera();
}

} // namespace Atrum