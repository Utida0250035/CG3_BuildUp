#pragma once

#include "Engine/Resource/LightData.h"

namespace Atrum::Visual {

// 平行光方向の簡易ライトシャフト（フォグと連動、将来ポストへ拡張）
struct GodRaySettings {
  Math::Vector3 color{1.0f, 0.92f, 0.78f};
  float intensity = 0.0f;
  float angularPower = 6.0f;
  float fogCoupling = 0.85f;
  uint32_t enabled = 0;
  float padding0 = 0.0f;

  void ApplyTo(DirectionalLightData &out) const;
  void ApplyTo(LightDataDirectional &out) const;

#ifdef USE_IMGUI

  void ImGui();

#endif
};

class GodRays final {
  GodRaySettings settings_{};

public:
  GodRaySettings &RefSettings() { return settings_; }
  const GodRaySettings &GetSettings() const { return settings_; }
  void SetSettings(const GodRaySettings &settings) { settings_ = settings; }

  void ApplyTo(DirectionalLightData &out) const { settings_.ApplyTo(out); }
  void ApplyTo(LightDataDirectional &out) const { settings_.ApplyTo(out); }

#ifdef USE_IMGUI

  void ImGui() { settings_.ImGui(); }

#endif
};

} // namespace Atrum::Visual
