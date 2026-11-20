#pragma once

#include "Engine/Renderer/Lighting/DirectionalShadow.h"
#include "Engine/Renderer/Lighting/GodRays.h"
#include "Engine/Renderer/Lighting/VolumetricFog.h"

namespace Atrum {

class AtrumEngine;

namespace Visual::LightingEngineEffectsBinding {

/// フォグ／ゴッドレイ／影の単一正本は AtrumEngine（Visual::Lighting は参照のみ）。
AtrumEngine *OwnerEngine();

VolumetricFog &SharedVolumetricFog();
GodRays &SharedGodRays();
DirectionalShadow &SharedDirectionalShadow();

} // namespace Visual::LightingEngineEffectsBinding
} // namespace Atrum