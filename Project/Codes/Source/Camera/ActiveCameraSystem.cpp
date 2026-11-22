#include "Camera/ActiveCameraSystem.h"

#include "Engine/AtrumEngine.h"
#include "Scene/Scene.h"

namespace Atrum {

ActiveCameraSystem &ActiveCameraSystem::Instance() {
  static ActiveCameraSystem instance;
  return instance;
}

void ActiveCameraSystem::PrepareSceneAndBindEngine(Scene *scene,
                                                   AtrumEngine *engine) {
  activeCamera_ = nullptr;
  if (scene == nullptr) {
    return;
  }
  scene->PrepareCamerasForDraw();
  activeCamera_ = scene->GetCamera();
  if (engine != nullptr && activeCamera_ != nullptr) {
    engine->SetCamera(activeCamera_);
  }
}

void ActiveCameraSystem::SyncEngineAfterSceneViewFit(AtrumEngine *engine,
                                                     Camera *camera) {
  activeCamera_ = camera;
  if (engine != nullptr && camera != nullptr) {
    engine->SetCamera(camera);
  }
}

} // namespace Atrum
