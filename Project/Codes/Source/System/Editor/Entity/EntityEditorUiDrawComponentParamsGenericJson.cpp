#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsGenericJson.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Editor/Entity/EntityEditorKnownComponentRegistry.h"

#include "ForDebug/ImGui.h"
#include <nlohmann/json.hpp>
#include <string>

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsGenericJson {

namespace {

constexpr int kIdStackId = 0x7F4A91;

bool DrawScalarField(const char *label, nlohmann::json &value, bool &changed) {
  if (value.is_boolean()) {
    bool flag = value.get<bool>();
    if (ImGui::Checkbox(label, &flag)) {
      value = flag;
      changed = true;
    }
    return true;
  }
  if (value.is_number_integer()) {
    int number = value.get<int>();
    if (ImGui::DragInt(label, &number, 1, -100000, 100000)) {
      value = number;
      changed = true;
    }
    return true;
  }
  if (value.is_number_float() || value.is_number_unsigned()) {
    float number = value.get<float>();
    if (ImGui::DragFloat(label, &number, 0.01f, -100000.0f, 100000.0f)) {
      value = number;
      changed = true;
    }
    return true;
  }
  if (value.is_string()) {
    std::string text = value.get<std::string>();
    if (ImGui::InputString(label, text)) {
      value = text;
      changed = true;
    }
    return true;
  }
  return false;
}

void DrawJsonObjectFields(nlohmann::json &object, bool &changed, const int depth) {
  if (!object.is_object()) {
    ImGui::TextDisabled("object 以外の params は JSON 手編集が必要です");
    return;
  }
  if (depth > 4) {
    ImGui::TextDisabled("ネストが深すぎます（JSON 手編集推奨）");
    return;
  }

  std::vector<std::string> keysToRemove;
  for (auto it = object.begin(); it != object.end(); ++it) {
    ImGui::PushID(it.key().c_str());
    nlohmann::json &child = it.value();
    if (child.is_object()) {
      if (ImGui::TreeNode(it.key().c_str())) {
        DrawJsonObjectFields(child, changed, depth + 1);
        ImGui::TreePop();
      }
    } else if (child.is_array()) {
      ImGui::TextDisabled("%s: 配列 %zu 件", it.key().c_str(), child.size());
    } else if (!DrawScalarField(it.key().c_str(), child, changed)) {
      ImGui::TextDisabled("%s: 未サポート型", it.key().c_str());
    }
    if (ImGui::SmallButton("削除")) {
      keysToRemove.push_back(it.key());
    }
    ImGui::PopID();
  }

  for (const std::string &key : keysToRemove) {
    object.erase(key);
    changed = true;
  }
}

} // namespace

bool TryDrawUnknownComponentParams(ComponentSlot &slot, bool &outChanged) {
  outChanged = false;
  if (EntityEditorKnownComponentRegistry::Contains(slot.typeName)) {
    return false;
  }

  ImGui::TextDisabled(
      "登録外 typeName（feature-memos/component_split_guidelines.md）");
  ImGui::Text("typeName: %s", slot.typeName.c_str());
  ImGui::TextDisabled("params を簡易 JSON ツリーで編集できます");

  if (!slot.params.is_object()) {
    slot.params = nlohmann::json::object();
    outChanged = true;
  }

  static char newKeyBuffer[64]{};
  ImGui::PushID(kIdStackId);
  DrawJsonObjectFields(slot.params, outChanged, 0);
  ImGui::InputText("新規キー", newKeyBuffer, sizeof(newKeyBuffer));
  ImGui::SameLine();
  if (ImGui::Button("空文字で追加") && newKeyBuffer[0] != '\0') {
    const std::string key{newKeyBuffer};
    if (!slot.params.contains(key)) {
      slot.params[key] = "";
      outChanged = true;
    }
    newKeyBuffer[0] = '\0';
  }
  ImGui::PopID();

  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsGenericJson