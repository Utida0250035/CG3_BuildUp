#include "Scene/SceneBuildUpStageCameraLock.h"

#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Scene/SceneBuildUp.h"

namespace Atrum::SceneBuildUpStageCameraLock {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

bool TryToggleLockHotkey(SceneBuildUp &scene) {
  if (!::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F5)) {
    return false;
  }
  scene.SetStageViewCameraLocked(
      !scene.IsStageViewCameraLocked());
  return true;
}

#endif

} // namespace Atrum::SceneBuildUpStageCameraLock