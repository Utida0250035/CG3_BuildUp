#include "Scene/SceneGameIntroBarrageAudio.h"

#include "System/Entity/Entity.h"
#include "System/OrbiterChildUtils.h"

namespace Atrum {

void UpdateSceneGameIntroBarrageAudio(Entity *boss) {
  if (boss == nullptr) {
    return;
  }
  OrbiterChildUtils::UpdateBossOrbiterMovementOnly(*boss);
  boss->Update();
}

} // namespace Atrum