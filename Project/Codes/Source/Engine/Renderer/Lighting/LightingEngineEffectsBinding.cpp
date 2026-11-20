#include "Engine/Renderer/Lighting/LightingEngineEffectsBinding.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"

namespace Atrum::Visual::LightingEngineEffectsBinding {

AtrumEngine *OwnerEngine() { return ::Atrum::EngineInstance(); }

VolumetricFog &SharedVolumetricFog() {
  AtrumEngine *engine = OwnerEngine();
  static VolumetricFog kFallback{};
  return engine != nullptr ? engine->RefVolumetricFog() : kFallback;
}

GodRays &SharedGodRays() {
  AtrumEngine *engine = OwnerEngine();
  static GodRays kFallback{};
  return engine != nullptr ? engine->RefGodRays() : kFallback;
}

DirectionalShadow &SharedDirectionalShadow() {
  AtrumEngine *engine = OwnerEngine();
  static DirectionalShadow kFallback{};
  return engine != nullptr ? engine->RefDirectionalShadow() : kFallback;
}

} // namespace Atrum::Visual::LightingEngineEffectsBinding