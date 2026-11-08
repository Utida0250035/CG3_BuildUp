#pragma once

#include "Engine/Resource/LightData.h"

namespace Atrum::Visual {

// シーン全体の高さ×距離フォグ（将来のフロクセル等へ拡張する統合設定）
struct VolumetricFogSettings {
  Math::Vector3 color{0.65f, 0.72f, 0.82f};
  float density = 0.0f;
  float heightFalloff = 0.35f;
  float heightOffset = 0.0f;
  float startDistance = 2.0f;
  float maxDistance = 120.0f;
  float inScatterIntensity = 0.25f;
  uint32_t enabled = 0;
  float padding0 = 0.0f;

  void ApplyTo(DirectionalLightData &out) const;
  void ApplyTo(LightDataDirectional &out) const;

#ifdef USE_IMGUI

  void ImGui();

#endif
};

class VolumetricFog final {
  VolumetricFogSettings settings_{};

public:
  VolumetricFogSettings &RefSettings() { return settings_; }
  const VolumetricFogSettings &GetSettings() const { return settings_; }
  void SetSettings(const VolumetricFogSettings &settings) { settings_ = settings; }

  void ApplyTo(DirectionalLightData &out) const { settings_.ApplyTo(out); }
  void ApplyTo(LightDataDirectional &out) const { settings_.ApplyTo(out); }

#ifdef USE_IMGUI

  void ImGui() { settings_.ImGui(); }

#endif
};

} // namespace Atrum::Visual
