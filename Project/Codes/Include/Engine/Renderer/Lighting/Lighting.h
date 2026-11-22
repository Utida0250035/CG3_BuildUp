#pragma once

#include "Engine/Renderer/Lighting/DirectionalShadow.h"
#include "Engine/Renderer/Lighting/GodRays.h"
#include "Engine/Renderer/Lighting/VolumetricFog.h"

namespace Atrum::Visual {

/// エンジンと同一の VolumetricFog / GodRays / Shadow を参照するファサード。
/// ランタイム CB 反映は AtrumEngine::FlushDirectionalLightConstantBuffer。
class Lighting final {
public:
  Lighting() = default;
  ~Lighting() = default;

  VolumetricFog &RefVolumetricFog();
  const VolumetricFog &GetVolumetricFog() const;

  GodRays &RefGodRays();
  const GodRays &GetGodRays() const;

  DirectionalShadow &RefDirectionalShadow();
  const DirectionalShadow &GetDirectionalShadow() const;

#ifdef USE_IMGUI

  void ImGui();

#endif

  Lighting(const Lighting &) = delete;
  Lighting &operator=(const Lighting &) = delete;
};

} // namespace Atrum::Visual