#include "Math/Vector4.h"
#include <cmath>

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

#include <nlohmann/json.hpp>

namespace Atrum::Math {

float Vector4::Length() const { return sqrt(LengthSquare()); }

#ifdef USE_IMGUI

void Vector4::DragGui(const char *label) {

  const float kThirtySecond = 0.03125f;

  ImGui::DragFloat4(label, &this->x, kThirtySecond);
}

void Vector4::ColorEdit(const char *label) {

  ImGui::ColorEdit4(label, &this->x);
}

#endif

void to_json(nlohmann::json &json, const Vector4 &vector) {
  json = nlohmann::json{
      {"x", vector.x}, {"y", vector.y}, {"z", vector.z}, {"w", vector.w}};
}

void from_json(const nlohmann::json &json, Vector4 &vector) {
  vector.x = json.value("x", vector.x);
  vector.y = json.value("y", vector.y);
  vector.z = json.value("z", vector.z);
  vector.w = json.value("w", vector.w);
}
} // namespace Atrum::Math