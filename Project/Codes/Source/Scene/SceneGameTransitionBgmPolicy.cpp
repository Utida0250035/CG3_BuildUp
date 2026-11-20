#include "Scene/SceneGameTransitionBgmPolicy.h"

#include "Scene/SceneGame.h"

namespace Atrum {

void MaintainSceneGameBgmDuringTransitionEffect(SceneGame &scene) {
  scene.MaintainBgmDuringSceneTransitionEffect();
}

} // namespace Atrum