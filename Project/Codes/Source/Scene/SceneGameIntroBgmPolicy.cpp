#include "Scene/SceneGameIntroBgmPolicy.h"

#include "Camera/CameraController/CameraGameIntroController.h"

namespace Atrum {

bool ShouldDeferSceneBgmRestartDuringIntro(
    const CameraGameIntroController *introController) {
  return introController != nullptr && !introController->IsFinished();
}

} // namespace Atrum