#include "Engine/Renderer/Lighting/Lighting.h"

#include "Engine/Renderer/Lighting/LightingEngineEffectsBinding.h"

namespace Atrum::Visual {

VolumetricFog &Lighting::RefVolumetricFog() {
  return LightingEngineEffectsBinding::SharedVolumetricFog();
}

const VolumetricFog &Lighting::GetVolumetricFog() const {
  return LightingEngineEffectsBinding::SharedVolumetricFog();
}

GodRays &Lighting::RefGodRays() {
  return LightingEngineEffectsBinding::SharedGodRays();
}

const GodRays &Lighting::GetGodRays() const {
  return LightingEngineEffectsBinding::SharedGodRays();
}

DirectionalShadow &Lighting::RefDirectionalShadow() {
  return LightingEngineEffectsBinding::SharedDirectionalShadow();
}

const DirectionalShadow &Lighting::GetDirectionalShadow() const {
  return LightingEngineEffectsBinding::SharedDirectionalShadow();
}

#ifdef USE_IMGUI

void Lighting::ImGui() {}

#endif

} // namespace Atrum::Visual