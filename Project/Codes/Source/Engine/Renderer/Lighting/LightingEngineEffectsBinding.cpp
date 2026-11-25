#include "Engine/Renderer/Lighting/LightingEngineEffectsBinding.h"

namespace Atrum::Visual::LightingEngineEffectsBinding {

VolumetricFog &SharedVolumetricFog() {
  static VolumetricFog kDetached{};
  return kDetached;
}

GodRays &SharedGodRays() {
  static GodRays kDetached{};
  return kDetached;
}

DirectionalShadow &SharedDirectionalShadow() {
  static DirectionalShadow kDetached{};
  return kDetached;
}

} // namespace Atrum::Visual::LightingEngineEffectsBinding