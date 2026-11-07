#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Entity/EntityEditor.h"

#include "ForDebug/ImGui.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/EntityType.h"

#include <algorithm>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

namespace {

struct KnownComponentEntry {
  const char *typeName;
  ComponentSlotKind kind;
};

constexpr KnownComponentEntry kKnownComponents[] = {
    {"CmpMeshSphere", ComponentSlotKind::Draw},
    {"CmpDrawHitSphere", ComponentSlotKind::Draw},
    {"CmpRotator", ComponentSlotKind::Upd},
    {"CmpScroll", ComponentSlotKind::Upd},
    {"CmpHitSphere", ComponentSlotKind::Upd},
};

EntityType EntityTypeFromIndex(const int index) {
  switch (index) {
  case 1:
    return EntityType::PLAYER;
  case 2:
    return EntityType::ENEMY;
  case 3:
    return EntityType::PLAYER_WEAPON;
  case 4:
    return EntityType::ENEMY_WEAPON;
  case 5:
    return EntityType::PLAYER_ITEM;
  case 6:
    return EntityType::ENEMY_ITEM;
  case 0:
  default:
    return EntityType::NONE;
  }
}

int EntityTypeToIndex(const EntityType type) {
  switch (type) {
  case EntityType::PLAYER:
    return 1;
  case EntityType::ENEMY:
    return 2;
  case EntityType::PLAYER_WEAPON:
    return 3;
  case EntityType::ENEMY_WEAPON:
    return 4;
  case EntityType::PLAYER_ITEM:
    return 5;
  case EntityType::ENEMY_ITEM:
    return 6;
  case EntityType::NONE:
  default:
    return 0;
  }
}

bool DrawVector3Param(const char *label, nlohmann::json &params,
                      const char *key, const Math::Vector3 &fallback) {
  Math::Vector3 v = params.value(key, fallback);
  const bool changed = ImGui::DragFloat3(label, &v.x, 0.02f);
  if (changed) {
    params[key] = v;
  }
  return changed;
}

bool DrawVector4ColorParam(const char *label, nlohmann::json &params,
                             const char *key, const Math::Vector4 &fallback) {
  Math::Vector4 color = params.value(key, fallback);
  const bool changed = ImGui::ColorEdit4(label, &color.x);
  if (changed) {
    params[key] = color;
  }
  return changed;
}

bool DrawComponentParams(ComponentSlot &slot) {
  nlohmann::json &p = slot.params;
  bool changed = false;

  if (slot.typeName == "CmpMeshSphere") {
    float radius = p.value("radius", 1.0f);
    if (ImGui::DragFloat("radius", &radius, 0.02f, 0.01f, 64.0f)) {
      p["radius"] = radius;
      changed = true;
    }
    int subdivision = static_cast<int>(p.value("subdivision", 8u));
    if (ImGui::DragInt("subdivision", &subdivision, 1, 3, 32)) {
      p["subdivision"] = static_cast<uint32_t>(subdivision);
      changed = true;
    }
    bool lighting = p.value("isLightingEnabled", true);
    if (ImGui::Checkbox("isLightingEnabled", &lighting)) {
      p["isLightingEnabled"] = lighting;
      changed = true;
    }
    changed |= DrawVector4ColorParam("color", p, "color", Math::Vector4::White());
    return changed;
  }

  if (slot.typeName == "CmpDrawHitSphere") {
    ImGui::TextDisabled(
        "同一 Entity に CmpHitSphere (Upd) が必要です");
    changed |= DrawVector4ColorParam("color", p, "color",
                                     Math::Vector4{0.2f, 0.85f, 1.0f, 1.0f});
    bool lighting = p.value("isLightingEnabled", false);
    if (ImGui::Checkbox("isLightingEnabled", &lighting)) {
      p["isLightingEnabled"] = lighting;
      changed = true;
    }
    return changed;
  }

  if (slot.typeName == "CmpRotator") {
    changed |= DrawVector3Param("rotateAxis", p, "rotateAxis", Math::Vector3::UpLh());
    float speed = p.value("rotateSpeed", 0.0f);
    if (ImGui::DragFloat("rotateSpeed", &speed, 0.05f, -64.0f, 64.0f)) {
      p["rotateSpeed"] = speed;
      changed = true;
    }
    bool pivotIsOwner = p.value("pivotIsOwner", true);
    if (ImGui::Checkbox("pivotIsOwner", &pivotIsOwner)) {
      p["pivotIsOwner"] = pivotIsOwner;
      changed = true;
    }
    if (pivotIsOwner) {
      ImGui::TextDisabled("プレビューではオーナー Entity をピボットにします");
    } else {
      ImGui::TextDisabled("ピボット未設定の場合、CmpRotator は動作しません");
    }
    return changed;
  }

  if (slot.typeName == "CmpScroll") {
    float speed = p.value("scrollSpeed", 0.0f);
    if (ImGui::DragFloat("scrollSpeed", &speed, 0.05f, -64.0f, 64.0f)) {
      p["scrollSpeed"] = speed;
      changed = true;
    }
    return changed;
  }

  if (slot.typeName == "CmpHitSphere") {
    bool active = p.value("isActive", true);
    if (ImGui::Checkbox("isActive", &active)) {
      p["isActive"] = active;
      changed = true;
    }
    if (!p.contains("spheres") || !p.at("spheres").is_array()) {
      p["spheres"] = nlohmann::json::array();
    }
    if (p.at("spheres").empty()) {
      p["spheres"].push_back(
          {{"localOffset", Math::Vector3::Zero()}, {"radius", 0.5f}});
    }
    nlohmann::json &sphere0 = p.at("spheres").at(0);
    Math::Vector3 offset =
        sphere0.value("localOffset", Math::Vector3::Zero());
    if (ImGui::DragFloat3("spheres[0].localOffset", &offset.x, 0.02f)) {
      sphere0["localOffset"] = offset;
      changed = true;
    }
    float radius = sphere0.value("radius", 0.5f);
    if (ImGui::DragFloat("spheres[0].radius", &radius, 0.02f, 0.01f, 64.0f)) {
      sphere0["radius"] = radius;
      changed = true;
    }
    ImGui::TextDisabled("現状 UI は当たり球 1 個分のみ編集します");
    return changed;
  }

  ImGui::TextDisabled("未対応コンポーネント: %s", slot.typeName.c_str());
  return false;
}

void ClampSelectedComponentIndex(UiState &ui, const EntityDefinition &document) {
  if (document.components.empty()) {
    ui.selectedComponentIndex = 0;
    return;
  }
  ui.selectedComponentIndex = std::clamp(
      ui.selectedComponentIndex, 0,
      static_cast<int>(document.components.size()) - 1);
}

} // namespace

void EntityEditor::DrawEditorUi() {
  if (!ImGui::Begin("Entity Editor")) {
    ImGui::End();
    return;
  }

  ImGui::TextUnformatted("F1: タイトルへ戻る");
  ImGui::Separator();

  ImGui::InputString("JSON path", ui_.jsonFilePath);
  if (ImGui::Button("Reload")) {
    ui_.requestReloadFromDisk = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    ui_.requestSaveToDisk = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Apply preview")) {
    ui_.requestApplyPreview = true;
  }

  ImGui::Separator();

  bool changed = false;
  changed |= ImGui::InputString("entityName", document_.entityName);

  const char *entityTypeNames[] = {
      "NONE",         "PLAYER",        "ENEMY",   "PLAYER_WEAPON",
      "ENEMY_WEAPON", "PLAYER_ITEM",   "ENEMY_ITEM"};
  int typeIndex = EntityTypeToIndex(document_.entityType);
  if (ImGui::Combo("entityType", &typeIndex, entityTypeNames,
                   IM_ARRAYSIZE(entityTypeNames))) {
    document_.entityType = EntityTypeFromIndex(typeIndex);
    changed = true;
  }

  if (ImGui::CollapsingHeader("initialTransform",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    const Math::TransformLH transformBefore = document_.initialTransform;
    document_.initialTransform.ImGui("initialTransform");
    const Math::TransformLH &transformAfter = document_.initialTransform;
    if (transformBefore.translate.x != transformAfter.translate.x ||
        transformBefore.translate.y != transformAfter.translate.y ||
        transformBefore.translate.z != transformAfter.translate.z ||
        transformBefore.scale.x != transformAfter.scale.x ||
        transformBefore.scale.y != transformAfter.scale.y ||
        transformBefore.scale.z != transformAfter.scale.z ||
        transformBefore.quaternion.x != transformAfter.quaternion.x ||
        transformBefore.quaternion.y != transformAfter.quaternion.y ||
        transformBefore.quaternion.z != transformAfter.quaternion.z ||
        transformBefore.quaternion.w != transformAfter.quaternion.w) {
      changed = true;
    }
  }

  ImGui::Separator();
  ImGui::TextUnformatted("Components");

  static int addPresetIndex = 0;
  const int presetCount = static_cast<int>(std::size(kKnownComponents));
  const char *presetLabels[presetCount];
  for (int i = 0; i < presetCount; ++i) {
    presetLabels[i] = kKnownComponents[i].typeName;
  }
  ImGui::Combo("add preset", &addPresetIndex, presetLabels, presetCount);
  if (ImGui::Button("Add component")) {
    const KnownComponentEntry &entry = kKnownComponents[addPresetIndex];
    ComponentSlot slot{};
    slot.kind = entry.kind;
    slot.typeName = entry.typeName;
    slot.params = DefaultParamsForType(entry.typeName);
    document_.components.push_back(std::move(slot));
    ui_.selectedComponentIndex =
        static_cast<int>(document_.components.size()) - 1;
    ui_.requestApplyPreview = true;
  }

  ClampSelectedComponentIndex(ui_, document_);
  if (!document_.components.empty() &&
      ImGui::Button("Remove selected component")) {
    document_.components.erase(
        document_.components.begin() + ui_.selectedComponentIndex);
    ClampSelectedComponentIndex(ui_, document_);
    ui_.requestApplyPreview = true;
  }

  if (document_.components.empty()) {
    ImGui::TextUnformatted(
        "コンポーネントがありません。Add component で追加してください。");
    if (changed) {
      ui_.requestApplyPreview = true;
    }
    ImGui::End();
    return;
  }

  for (int i = 0; i < static_cast<int>(document_.components.size()); ++i) {
    const ComponentSlot &slot = document_.components[static_cast<size_t>(i)];
    const std::string label = std::to_string(i) + ": " + slot.typeName;
    if (ImGui::Selectable(label.c_str(), ui_.selectedComponentIndex == i)) {
      ui_.selectedComponentIndex = i;
    }
  }

  ImGui::Separator();
  ComponentSlot &selected =
      document_.components[static_cast<size_t>(ui_.selectedComponentIndex)];

  const char *kindNames[] = {"Upd", "Draw", "Data"};
  int kindIndex = 0;
  if (selected.kind == ComponentSlotKind::Draw) {
    kindIndex = 1;
  } else if (selected.kind == ComponentSlotKind::Data) {
    kindIndex = 2;
  }
  ImGui::TextDisabled("kind: %s", kindNames[kindIndex]);

  static int typeComboIndex = 0;
  for (int i = 0; i < presetCount; ++i) {
    if (selected.typeName == kKnownComponents[i].typeName) {
      typeComboIndex = i;
      break;
    }
  }
  if (ImGui::Combo("typeName", &typeComboIndex, presetLabels, presetCount)) {
    const KnownComponentEntry &entry = kKnownComponents[typeComboIndex];
    selected.kind = entry.kind;
    selected.typeName = entry.typeName;
    selected.params = DefaultParamsForType(entry.typeName);
    changed = true;
  }

  if (DrawComponentParams(selected)) {
    changed = true;
  }

  if (changed) {
    ui_.requestApplyPreview = true;
  }

  ImGui::End();
}

} // namespace Atrum::EntityEditor

#endif
