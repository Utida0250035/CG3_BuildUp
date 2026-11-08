#include "Engine/Renderer/Lighting/DirectionalShadow.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "System/WhiteTexture.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Visual {

void DirectionalShadowSettings::ApplyTo(DirectionalLightData &out) const {
  out.shadowShadeEnabled = shadeEnabled;
  out.shadowShadeDarkness = shadeDarkness;
  out.shadowGroundPlaneY = groundPlaneY;
  out.shadowContactHeight = contactHeight;
}

void DirectionalShadowSettings::ApplyTo(LightDataDirectional &out) const {
  out.shadowShadeEnabled = shadeEnabled;
  out.shadowShadeDarkness = shadeDarkness;
  out.shadowGroundPlaneY = groundPlaneY;
  out.shadowContactHeight = contactHeight;
  if (shadeEnabled != 0u || planarEnabled != 0u) {
    out.flags |= static_cast<uint32_t>(LightFlags::CAST_SHADOW);
  } else {
    out.flags &= ~static_cast<uint32_t>(LightFlags::CAST_SHADOW);
  }
}

void DirectionalShadowSettings::DrawPlanarGroundShadow(
    AtrumEngine *engine, const Math::Vector3 &worldAnchor,
    const float footprintRadius) const {
  if (engine == nullptr || planarEnabled == 0u || footprintRadius <= 0.0f) {
    return;
  }

  Math::TransformLH shadowTransform{};
  shadowTransform.translate = {worldAnchor.x, groundPlaneY, worldAnchor.z};
  shadowTransform.scale = {1.0f, planarSquash, 1.0f};

  const float drawRadius = footprintRadius * planarRadiusScale;
  const Math::Vector4 shadowColor{0.0f, 0.0f, 0.0f, planarAlpha};

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->DrawSphere(WhiteTextureIndex(), shadowColor, Math::Transform{},
                     shadowTransform.MakeWorldMatrix(), drawRadius, 2, false);
}

#ifdef USE_IMGUI

void DirectionalShadowSettings::ImGui() {
  constexpr float kStep = 0.03125f;

  if (ImGui::TreeNode("Directional shade (shader)")) {
    bool shadeOn = shadeEnabled != 0u;
    if (ImGui::Checkbox("shade enabled", &shadeOn)) {
      shadeEnabled = shadeOn ? 1u : 0u;
    }
    ImGui::DragFloat("darkness", &shadeDarkness, kStep, 0.0f, 1.0f);
    ImGui::DragFloat("ground plane Y", &groundPlaneY, kStep, -64.0f, 64.0f);
    ImGui::DragFloat("contact height", &contactHeight, kStep, 0.1f, 32.0f);
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Planar ground shadow")) {
    bool planarOn = planarEnabled != 0u;
    if (ImGui::Checkbox("planar enabled", &planarOn)) {
      planarEnabled = planarOn ? 1u : 0u;
    }
    ImGui::DragFloat("alpha", &planarAlpha, kStep, 0.0f, 1.0f);
    ImGui::DragFloat("squash", &planarSquash, kStep, 0.02f, 0.5f);
    ImGui::DragFloat("radius scale", &planarRadiusScale, kStep, 0.25f, 3.0f);
    ImGui::TreePop();
  }
}

#endif

} // namespace Atrum::Visual
