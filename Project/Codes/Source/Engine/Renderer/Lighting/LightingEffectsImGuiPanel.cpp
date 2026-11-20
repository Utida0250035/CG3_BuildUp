#include "Engine/Renderer/Lighting/LightingEffectsImGuiPanel.h"

#if defined(USE_IMGUI)

#include "Engine/Renderer/Lighting/DirectionalShadow.h"
#include "Engine/Renderer/Lighting/GodRays.h"
#include "Engine/Renderer/Lighting/VolumetricFog.h"
#include "ForDebug/ImGui.h"

namespace Atrum::Visual {

void DrawLightingEffectsImGuiPanel(VolumetricFog &volumetricFog, GodRays &godRays,
                                 DirectionalShadow &directionalShadow) {
  if (ImGui::CollapsingHeader("VolumetricFog", ImGuiTreeNodeFlags_DefaultOpen)) {
    volumetricFog.ImGui();
  }
  if (ImGui::CollapsingHeader("GodRays", ImGuiTreeNodeFlags_DefaultOpen)) {
    godRays.ImGui();
  }
  if (ImGui::CollapsingHeader("Shadow", ImGuiTreeNodeFlags_DefaultOpen)) {
    directionalShadow.ImGui();
  }
}

} // namespace Atrum::Visual

#endif