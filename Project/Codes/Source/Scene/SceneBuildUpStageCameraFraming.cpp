#include "Scene/SceneBuildUpStageCameraFraming.h"

#include "Scene/SceneBuildUpSceneDocumentLoad.h"
#include "Scene/Scene.h"

namespace Atrum::SceneBuildUpStageCamera {

void ApplyFramingFromSceneDocument(
    Scene &scene, const EntityEditor::EditorSceneDocument &document) {
  scene.SetStageViewFramingBounds(
      SceneBuildUpSceneDocumentLoad::ResolveStageViewFraming(document));
}

void ApplyStageCameraAfterSceneLoad(Scene &scene) {
  PublishStageViewFramingBounds(scene.GetStageViewFramingBounds());
  if (scene.IsStageViewCameraLocked()) {
    scene.SetupStageViewCamera();
  }
}

void MaintainLockedStageCamera(Scene &scene) {
  PublishStageViewFramingBounds(scene.GetStageViewFramingBounds());
  if (scene.IsStageViewCameraLocked()) {
    scene.SetupStageViewCamera();
  }
  scene.PrepareCamerasForDraw();
}

} // namespace Atrum::SceneBuildUpStageCamera