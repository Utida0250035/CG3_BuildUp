#include "Scene/SceneBuildUpRuntimeSubviewDrawCamera.h"

#include "Camera/Camera.h"
#include "Math/Matrix4x4.h"
#include "Scene/Scene.h"
#include "System/Editor/EditorFrameFitCameraPerspectiveParams.h"

namespace Atrum::SceneBuildUpRuntimeSubviewDrawCamera {

namespace {

void FitCameraPerspectiveToViewport(Camera *camera, const int width,
                                    const int height) {
  if (camera == nullptr || width <= 0 || height <= 0) {
    return;
  }
  using Editor::EditorFrameFitCameraPerspectiveParams::kViewportFarClip;
  using Editor::EditorFrameFitCameraPerspectiveParams::kViewportNearClip;
  using Editor::EditorFrameFitCameraPerspectiveParams::ResolveViewportPerspectiveFovY;

  const float aspect =
      static_cast<float>(width) / static_cast<float>(height);
  const float fovY = ResolveViewportPerspectiveFovY();
  camera->SetPerspectiveFovMatrix(Math::Matrix4x4::PerspectiveFov(
      fovY, aspect, kViewportNearClip, kViewportFarClip));
  camera->UpdateMatrix();
}

} // namespace

Camera *BindAfterGameOrderPrepare(Scene &scene, int viewportWidth,
                                  int viewportHeight) {
  scene.PrepareCamerasForDraw();
  Atrum::Camera *camera = scene.GetCamera();
  if (camera == nullptr || viewportWidth <= 0 || viewportHeight <= 0) {
    return camera;
  }
  FitCameraPerspectiveToViewport(camera, viewportWidth, viewportHeight);
  return camera;
}

} // namespace Atrum::SceneBuildUpRuntimeSubviewDrawCamera