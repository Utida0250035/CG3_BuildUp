#include "Engine/Resource/LightData.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum {

#ifdef USE_IMGUI

void DirectionalLightData::ImGui() {

  constexpr float kThirtySecond = 0.03125f;

  if (ImGui::BeginChild("DirectionalLight")) {

    ImGui::DragFloat3("color", &color.x, kThirtySecond, 0.0f, 1.0f);

    ImGui::DragFloat3("direction", &direction.x, kThirtySecond);

    if (ImGui::IsItemActive()) {

      direction.Normalize();
    }

    ImGui::DragFloat("intensity", &intensity, kThirtySecond);

    int lightMode = static_cast<int>(lightModel);

    ImGui::Selectable("lambert", lightMode == 0);
    if (ImGui::IsItemActivated()) {

      lightMode = 0;
    }

    ImGui::Selectable("halfLambert", lightMode == 1);
    if (ImGui::IsItemActivated()) {

      lightMode = 1;
    }

    lightModel = static_cast<LightModel>(lightMode);
  }

  ImGui::EndChild();
}

#endif
} // namespace Atrum