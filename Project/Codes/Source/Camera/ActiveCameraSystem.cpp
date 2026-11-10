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

} // namespace Atrum
