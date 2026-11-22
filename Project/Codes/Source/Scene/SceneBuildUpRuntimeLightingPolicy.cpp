#include "Scene/SceneBuildUpRuntimeLightingPolicy.h"

#include "Scene/SceneBuildUpRuntimeVisualApply.h"

namespace Atrum::SceneBuildUpRuntimeLightingPolicy {

void ApplyOnceOnEnterScene() {
  ApplySceneBuildUpRuntimeVisualFromProfile();
}

} // namespace Atrum::SceneBuildUpRuntimeLightingPolicy