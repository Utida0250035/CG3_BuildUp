#include "Scene/SceneBuildUpRuntimeSubviewDrawCamera.h"

#include "Camera/Camera.h"
#include "Scene/Scene.h"
#include "System/Editor/EditorFrame.h"

namespace Atrum::SceneBuildUpRuntimeSubviewDrawCamera {

Camera *BindAfterGameOrderPrepare(Scene &scene, int viewportWidth,
                                  int viewportHeight) {
  scene.PrepareCamerasForDraw();
  Atrum::Camera *camera = scene.GetCamera();
  if (camera == nullptr || viewportWidth <= 0 || viewportHeight <= 0) {
    return camera;
  }
  Editor::FitCameraPerspectiveToViewport(camera, viewportWidth, viewportHeight);
  return camera;
}

} // namespace Atrum::SceneBuildUpRuntimeSubviewDrawCamera