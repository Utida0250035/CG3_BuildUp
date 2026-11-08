#include "Engine/Renderer/Lighting/GodRays.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Visual {

void GodRaySettings::ApplyTo(DirectionalLightData &out) const {
  out.godRayColor = color;
  out.godRayIntensity = intensity;
  out.godRayAngularPower = angularPower;
  out.godRayFogCoupling = fogCoupling;
  out.godRayEnabled = enabled;
}

void GodRaySettings::ApplyTo(LightDataDirectional &out) const {
  out.godRayColor = color;
  out.godRayIntensity = intensity;
  out.godRayAngularPower = angularPower;
  out.godRayFogCoupling = fogCoupling;
  out.godRayEnabled = enabled;
}

#ifdef USE_IMGUI

void GodRaySettings::ImGui() {
  constexpr float kStep = 0.03125f;

  bool on = enabled != 0u;
  if (ImGui::Checkbox("god rays enabled", &on)) {
    enabled = on ? 1u : 0u;
  }

  ImGui::DragFloat3("ray color", &color.x, kStep, 0.0f, 4.0f);
  ImGui::DragFloat("intensity", &intensity, kStep, 0.0f, 4.0f);
  ImGui::DragFloat("angular power", &angularPower, kStep, 1.0f, 32.0f);
  ImGui::DragFloat("fog coupling", &fogCoupling, kStep, 0.0f, 1.0f);
}

#endif

} // namespace Atrum::Visual
