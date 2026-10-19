#include "Math/Transform.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

#include <nlohmann/json.hpp>

namespace Atrum::Math {

#ifdef USE_IMGUI

void Transform::ImGui(const char *label) {

  constexpr float kThirtySecond = 0.03125f;

  if (ImGui::BeginChild(label, ImGui::kChildSize, ImGui::kChildFlags)) {

    ImGui::Text(label);

    ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
    ImGui::DragFloat3("rotate", &rotate.x, kThirtySecond);
    ImGui::DragFloat3("translate", &translate.x, kThirtySecond);
  }

  ImGui::EndChild();
}

#endif

void to_json(nlohmann::json &json, const Transform &transform) {}

void from_json(const nlohmann::json &json, Transform &transform) {}

#ifdef USE_IMGUI

void TransformLH::ImGui(const char *label) {

  constexpr float kThirtySecond = 0.03125f;

  if (ImGui::BeginChild(label, ImGui::kChildSize, ImGui::kChildFlags)) {

    ImGui::Text(label);

    ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
    ImGui::DragFloat4("quaternion", &quaternion.x, kThirtySecond);

    if (ImGui::IsItemActive()) {

      quaternion.Normalize();
    }

    ImGui::DragFloat3("translate", &translate.x, kThirtySecond);
  }

  ImGui::EndChild();
}

#endif

void to_json(nlohmann::json &json, const TransformLH &transform) {}
void from_json(const nlohmann::json &json, TransformLH &transform) {}

#ifdef USE_IMGUI

void TransformRH::ImGui(const char *label) {

  constexpr float kThirtySecond = 0.03125f;

  if (ImGui::BeginChild(label, ImGui::kChildSize, ImGui::kChildFlags)) {

    ImGui::Text(label);

    ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
    ImGui::DragFloat4("quaternion", &quaternion.x, kThirtySecond);

    if (ImGui::IsItemActive()) {

      quaternion.Normalize();
    }

    ImGui::DragFloat3("translate", &translate.x, kThirtySecond);
  }

  ImGui::EndChild();
}

#endif

void to_json(nlohmann::json &json, const TransformRH &transform);
void from_json(const nlohmann::json &json, TransformRH &transform);

} // namespace Atrum::Math