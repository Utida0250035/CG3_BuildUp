#include "Scene/SceneBuildUpStageCameraFraming.h"

#include "Camera/StageViewFramingBounds.h"
#include "Scene/Scene.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"

namespace Atrum::SceneBuildUpStageCamera {

void ApplyFramingFromSceneDocument(
    Scene &scene, const EntityEditor::EditorSceneDocument &document) {
  scene.SetStageViewFramingBounds(
      EntityEditor::ResolveStageViewFramingBounds(document));
}

void MaintainLockedStageCamera(Scene &scene) {
  PublishStageViewFramingBounds(scene.GetStageViewFramingBounds());
  scene.SetupStageViewCamera();
  scene.PrepareCamerasForDraw();
}

} // namespace Atrum::SceneBuildUpStageCamera