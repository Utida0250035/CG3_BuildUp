#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsBallSplit.h"

#include "Math/Vector3.h"
#include "ForDebug/ImGui.h"
#include "Math/Vector3.h"

#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace {

bool DrawVector3Param(const char *label, nlohmann::json &params, const char *key,
                      const Math::Vector3 &fallback) {
  Math::Vector3 v = params.value(key, fallback);
  const bool changed = ImGui::DragFloat3(label, &v.x, 0.02f);
  if (changed) {
    params[key] = v;
  }
  return changed;
}

} // namespace

bool TryDrawBallSplitComponentParams(ComponentSlot &slot, bool &outChanged) {
  outChanged = false;
  nlohmann::json &p = slot.params;
  bool changed = false;

  if (slot.typeName == "CmpBallKinematics") {
    changed |= DrawVector3Param("velocity", p, "velocity", {7.0f, 5.5f, 0.0f});
    float gravity = p.value("gravity", -14.0f);
    if (ImGui::DragFloat("gravity", &gravity, 0.1f, -80.0f, 0.0f)) {
      p["gravity"] = gravity;
      changed = true;
    }
    outChanged = changed;
    return true;
  }

  if (slot.typeName == "CmpBallAxisAlignedBounce") {
    float leftWall = p.value("leftWallX", -5.0f);
    if (ImGui::DragFloat("leftWallX", &leftWall, 0.05f, -32.0f, 32.0f)) {
      p["leftWallX"] = leftWall;
      changed = true;
    }
    float rightWall = p.value("rightWallX", 5.0f);
    if (ImGui::DragFloat("rightWallX", &rightWall, 0.05f, -32.0f, 32.0f)) {
      p["rightWallX"] = rightWall;
      changed = true;
    }
    float groundY = p.value("groundY", -3.68f);
    if (ImGui::DragFloat("groundY", &groundY, 0.05f, -16.0f, 16.0f)) {
      p["groundY"] = groundY;
      changed = true;
    }
    float radius = p.value("radius", 0.32f);
    if (ImGui::DragFloat("radius", &radius, 0.01f, 0.05f, 4.0f)) {
      p["radius"] = radius;
      changed = true;
    }
    float wallRest = p.value("wallRestitution", 0.94f);
    if (ImGui::DragFloat("wallRestitution", &wallRest, 0.01f, 0.0f, 1.0f)) {
      p["wallRestitution"] = wallRest;
      changed = true;
    }
    float floorRest = p.value("floorRestitution", 0.88f);
    if (ImGui::DragFloat("floorRestitution", &floorRest, 0.01f, 0.0f, 1.0f)) {
      p["floorRestitution"] = floorRest;
      changed = true;
    }
    outChanged = changed;
    return true;
  }

  if (slot.typeName == "CmpBallBounceImpactFx") {
    std::string impactPath = p.value("impactJsonPath", std::string{});
    if (ImGui::InputString("impactJsonPath", impactPath)) {
      p["impactJsonPath"] = impactPath;
      changed = true;
    }
    float cooldown = p.value("impactCooldownSeconds", 0.12f);
    if (ImGui::DragFloat("impactCooldownSeconds", &cooldown, 0.01f, 0.0f,
                        4.0f)) {
      p["impactCooldownSeconds"] = cooldown;
      changed = true;
    }
    outChanged = changed;
    return true;
  }

  return false;
}

#else

bool TryDrawBallSplitComponentParams(ComponentSlot &, bool &outChanged) {
  outChanged = false;
  return false;
}

#endif

} // namespace Atrum::EntityEditor