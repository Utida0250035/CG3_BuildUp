#include "Scene/SceneResultExitBgmPolicy.h"

#include "Scene/SceneGameExitBgmPolicy.h"

namespace Atrum {

void ReleaseSceneResultBgmOnExitDuringFade(Audio::Handle &sceneBgmHandle) {
  ReleaseSceneBgmOnExitDuringFade(sceneBgmHandle);
}

} // namespace Atrum