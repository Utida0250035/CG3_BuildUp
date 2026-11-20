#include "Scene/SceneDebugCameraInputPolicy.h"

#include "Scene/Scene.h"

namespace Atrum {

bool AllowsPlayerDebugCameraInput(const Scene &scene) {
  if (scene.IsStageViewCameraLocked()) {
    return false;
  }
  return scene.AllowDebugCameraControl();
}

} // namespace Atrum