#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsCmpCamera.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Editor/Entity/EntityEditorDefaultParamsCmpCamera.h"

#include <ForDebug/ImGui.h>

#include <algorithm>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

bool TryDrawCmpCameraComponentParams(ComponentSlot &slot, bool &outChanged) {
  if (slot.typeName == "CmpCamera") {
  outChanged = false;
  nlohmann::json &p = slot.params;
  bool changed = false;

  ImGui::TextDisabled(
      "主カメラは camera.main タグ付きエンティティの CmpCamera として解決されます");

  const nlohmann::json defaults =
      EntityEditorDefaultParamsCmpCamera::Json();
  float fovY = p.value("perspectiveFovY",
                       defaults.value("perspectiveFovY", 0.5f));
  if (ImGui::DragFloat("perspectiveFovY", &fovY, 0.01f, 0.1f, 2.5f)) {
    p["perspectiveFovY"] = std::max(0.1f, fovY);
    changed = true;
  }
  float nearClip =
      p.value("nearClip", defaults.value("nearClip", 0.125f));
  if (ImGui::DragFloat("nearClip", &nearClip, 0.01f, 0.01f, 8.0f)) {
    p["nearClip"] = std::max(0.001f, nearClip);
    changed = true;
  }
  float farClip = p.value("farClip", defaults.value("farClip", 128.0f));
  if (ImGui::DragFloat("farClip", &farClip, 1.0f, 1.0f, 512.0f)) {
    p["farClip"] = std::max(nearClip + 0.01f, farClip);
    changed = true;
  }

  outChanged = changed;
  return true;
  }
  return false;
}

#else

bool TryDrawCmpCameraComponentParams(ComponentSlot &, bool &) { return false; }

#endif

} // namespace Atrum::EntityEditor