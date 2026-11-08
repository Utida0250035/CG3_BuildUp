#include "Engine/Renderer/Lighting/VolumetricFog.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Visual {

namespace {

void CopyFogFields(const VolumetricFogSettings &src, Math::Vector3 &fogColor,
                   float &fogDensity, float &fogHeightFalloff, float &fogHeightOffset,
                   float &fogStartDistance, float &fogMaxDistance,
                   float &fogInScatterIntensity, uint32_t &fogEnabled) {
  fogColor = src.color;
  fogDensity = src.density;
  fogHeightFalloff = src.heightFalloff;
  fogHeightOffset = src.heightOffset;
  fogStartDistance = src.startDistance;
  fogMaxDistance = src.maxDistance;
  fogInScatterIntensity = src.inScatterIntensity;
  fogEnabled = src.enabled;
}

} // namespace

void VolumetricFogSettings::ApplyTo(DirectionalLightData &out) const {
  CopyFogFields(*this, out.fogColor, out.fogDensity, out.fogHeightFalloff,
                out.fogHeightOffset, out.fogStartDistance, out.fogMaxDistance,
                out.fogInScatterIntensity, out.fogEnabled);
}

void VolumetricFogSettings::ApplyTo(LightDataDirectional &out) const {
  CopyFogFields(*this, out.fogColor, out.fogDensity, out.fogHeightFalloff,
                out.fogHeightOffset, out.fogStartDistance, out.fogMaxDistance,
                out.fogInScatterIntensity, out.fogEnabled);
  if (enabled != 0u) {
    out.flags |= static_cast<uint32_t>(LightFlags::VOLUMETRIC);
  } else {
    out.flags &= ~static_cast<uint32_t>(LightFlags::VOLUMETRIC);
  }
}

#ifdef USE_IMGUI

void VolumetricFogSettings::ImGui() {
  constexpr float kStep = 0.03125f;

  bool fogOn = enabled != 0u;
  if (ImGui::Checkbox("fog enabled", &fogOn)) {
    enabled = fogOn ? 1u : 0u;
  }

  ImGui::DragFloat3("fog color", &color.x, kStep, 0.0f, 4.0f);
  ImGui::DragFloat("density", &density, kStep * 0.25f, 0.0f, 0.2f);
  ImGui::DragFloat("height falloff", &heightFalloff, kStep, 0.0f, 4.0f);
  ImGui::DragFloat("height offset", &heightOffset, kStep, -64.0f, 64.0f);
  ImGui::DragFloat("start distance", &startDistance, kStep, 0.0f, 256.0f);
  ImGui::DragFloat("max distance", &maxDistance, kStep, 1.0f, 512.0f);
  ImGui::DragFloat("in-scatter", &inScatterIntensity, kStep, 0.0f, 2.0f);
}

#endif

} // namespace Atrum::Visual
