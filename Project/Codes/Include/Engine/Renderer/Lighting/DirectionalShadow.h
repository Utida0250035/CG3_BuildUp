#pragma once

#include "Engine/EngineFwd.h"
#include "Engine/Resource/LightData.h"
#include "Math/Vector3.h"

namespace Atrum {

class AtrumEngine;

namespace Visual {

struct DirectionalShadowSettings {
  uint32_t shadeEnabled = 0;
  float shadeDarkness = 0.45f;
  float groundPlaneY = 0.0f;
  float contactHeight = 2.5f;

  uint32_t planarEnabled = 0;
  float planarAlpha = 0.38f;
  float planarSquash = 0.14f;
  float planarRadiusScale = 1.05f;

  void ApplyTo(DirectionalLightData &out) const;
  void ApplyTo(LightDataDirectional &out) const;

  void DrawPlanarGroundShadow(AtrumEngine *engine, const Math::Vector3 &worldAnchor,
                              float footprintRadius) const;

#ifdef USE_IMGUI

  void ImGui();

#endif
};

class DirectionalShadow final {
  DirectionalShadowSettings settings_{};

public:
  DirectionalShadowSettings &RefSettings() { return settings_; }
  const DirectionalShadowSettings &GetSettings() const { return settings_; }
  void SetSettings(const DirectionalShadowSettings &settings) { settings_ = settings; }

  void ApplyTo(DirectionalLightData &out) const { settings_.ApplyTo(out); }
  void ApplyTo(LightDataDirectional &out) const { settings_.ApplyTo(out); }

  void DrawPlanarGroundShadow(AtrumEngine *engine, const Math::Vector3 &worldAnchor,
                              float footprintRadius) const {
    settings_.DrawPlanarGroundShadow(engine, worldAnchor, footprintRadius);
  }

#ifdef USE_IMGUI

  void ImGui() { settings_.ImGui(); }

#endif
};

} // namespace Visual
} // namespace Atrum
