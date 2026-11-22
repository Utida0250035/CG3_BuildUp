#include "Scene/SceneGameEnterVisualPreset.h"

#include "Engine/AtrumEngine.h"
#include "Scene/SceneGameLightingProfile.h"

namespace Atrum {

void ApplySceneGameVisualPresetOnEnter() {
  ApplySceneGameLightingFromProfile();
  if (Engine *const engine = Engine::Instance()) {
    engine->FlushDirectionalLightConstantBuffer();
  }
}

} // namespace Atrum