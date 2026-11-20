#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorSceneViewRenderCameraBind.h"

#include "Camera/ActiveCameraSystem.h"
#include "Engine/AtrumEngine.h"
#include "System/Editor/EditorFrame.h"

namespace Atrum::SceneEntityEditorSceneViewRenderCameraBind {

void BindForSceneViewRender(AtrumEngine *engine, Camera *sceneCamera,
                            const int width, const int height) {
  if (engine == nullptr || sceneCamera == nullptr || width <= 0 ||
      height <= 0) {
    return;
  }
  Camera *camera = ActiveCameraSystem::Instance().GetActiveCamera();
  if (camera == nullptr) {
    camera = sceneCamera;
  }
  Editor::FitCameraPerspectiveToViewport(camera, width, height);
  ActiveCameraSystem::Instance().SyncEngineAfterSceneViewFit(engine, camera);
}

} // namespace Atrum::SceneEntityEditorSceneViewRenderCameraBind

#endif