#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsFallbackJson.h"

#include "ForDebug/ImGui.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <cfloat>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsFallbackJson {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace {

constexpr int kFallbackIdStack = 0x5A11BA;

struct FallbackRawJsonState {
  const ComponentSlot *lastSlot = nullptr;
  std::string lastTypeName{};
  std::string jsonBuffer{};
  std::string parseError{};
};

FallbackRawJsonState &RawJsonState() {
  static FallbackRawJsonState state{};
  return state;
}

void SyncRawBufferFromSlot(FallbackRawJsonState &state,
                           const ComponentSlot &slot) {
  state.lastSlot = &slot;
  state.lastTypeName = slot.typeName;
  state.jsonBuffer = slot.params.dump(2);
  state.parseError.clear();
}

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

void DrawJsonValueRecursive(const char *label, nlohmann::json &value,
                            bool &changed, const int depth);

void DrawJsonArrayFields(nlohmann::json &arrayValue, bool &changed,
                         const int depth) {
  if (depth > 5) {
    ImGui::TextDisabled("ネストが深すぎます（生 JSON で編集してください）");
    return;
  }

  int removeIndex = -1;
  const int count = static_cast<int>(arrayValue.size());
  for (int i = 0; i < count; ++i) {
    ImGui::PushID(i);
    const std::string elemLabel = "[" + std::to_string(i) + "]";
    DrawJsonValueRecursive(elemLabel.c_str(),
                           arrayValue.at(static_cast<size_t>(i)), changed,
                           depth + 1);
    ImGui::SameLine();
    if (ImGui::SmallButton("削除")) {
      removeIndex = i;
    }
    ImGui::PopID();
  }

  if (removeIndex >= 0 && removeIndex < static_cast<int>(arrayValue.size())) {
    arrayValue.erase(arrayValue.begin() + removeIndex);
    changed = true;
  }

  if (ImGui::SmallButton("要素を追加 (0.0)")) {
    arrayValue.push_back(0.0f);
    changed = true;
  }
  ImGui::SameLine();
  if (ImGui::SmallButton("文字列要素を追加")) {
    arrayValue.push_back("");
    changed = true;
  }
}

void DrawJsonObjectFields(nlohmann::json &objectValue, bool &changed,
                          const int depth) {
  if (depth > 5) {
    ImGui::TextDisabled("ネストが深すぎます（生 JSON で編集してください）");
    return;
  }

  std::vector<std::string> keysToRemove;
  for (auto it = objectValue.begin(); it != objectValue.end(); ++it) {
    ImGui::PushID(it.key().c_str());
    DrawJsonValueRecursive(it.key().c_str(), it.value(), changed, depth + 1);
    ImGui::SameLine();
    if (ImGui::SmallButton("削除")) {
      keysToRemove.push_back(it.key());
    }
    ImGui::PopID();
  }

  for (const std::string &key : keysToRemove) {
    objectValue.erase(key);
    changed = true;
  }
}

void DrawJsonValueRecursive(const char *label, nlohmann::json &value,
                            bool &changed, const int depth) {
  if (value.is_object()) {
    if (ImGui::TreeNode(label)) {
      DrawJsonObjectFields(value, changed, depth);
      ImGui::TreePop();
    }
    return;
  }
  if (value.is_array()) {
    const std::string header =
        std::string(label) + " [" + std::to_string(value.size()) + "]";
    if (ImGui::TreeNode(label, "%s", header.c_str())) {
      DrawJsonArrayFields(value, changed, depth);
      ImGui::TreePop();
    }
    return;
  }
  if (!DrawScalarField(label, value, changed)) {
    ImGui::TextDisabled("%s: 未サポート値型（生 JSON で編集可）", label);
  }
}

nlohmann::json MakeInitialValueForKindIndex(const int kindIndex) {
  switch (kindIndex) {
  case 0:
    return std::string{};
  case 1:
    return 0.0f;
  case 2:
    return 0;
  case 3:
    return false;
  case 4:
    return nlohmann::json::object();
  case 5:
  default:
    return nlohmann::json::array();
  }
}

} // namespace

void DrawFallbackComponentParams(ComponentSlot &slot, bool &outChanged) {
  outChanged = false;
  bool changed = false;

  ImGui::TextColored(
      ImVec4(1.0f, 0.85f, 0.4f, 1.0f),
      "専用インスペクタ未対応コンポーネント: %s", slot.typeName.c_str());
  ImGui::TextDisabled(
      "汎用 JSON エディタおよび生 JSON テキストからパラメータを編集できます");

  const nlohmann::json defaultParams = DefaultParamsForType(slot.typeName);
  if (!defaultParams.empty() && ImGui::Button("既定 params にリセット")) {
    slot.params = defaultParams;
    changed = true;
  }

  if (!slot.params.is_object()) {
    slot.params = nlohmann::json::object();
    changed = true;
  }

  FallbackRawJsonState &rawState = RawJsonState();
  if (rawState.lastSlot != &slot || rawState.lastTypeName != slot.typeName) {
    SyncRawBufferFromSlot(rawState, slot);
  }

  ImGui::PushID(kFallbackIdStack);
  DrawJsonObjectFields(slot.params, changed, 0);

  static char newKeyBuffer[64]{};
  static int newValueKindIndex = 0;
  constexpr const char *kKindLabels[] = {"string", "float", "int",
                                         "bool",   "object", "array"};
  ImGui::Separator();
  ImGui::SetNextItemWidth(160.0f);
  ImGui::InputText("追加キー", newKeyBuffer, sizeof(newKeyBuffer));
  ImGui::SameLine();
  ImGui::SetNextItemWidth(90.0f);
  ImGui::Combo("型", &newValueKindIndex, kKindLabels,
               static_cast<int>(std::size(kKindLabels)));
  ImGui::SameLine();
  if (ImGui::Button("キーを追加") && newKeyBuffer[0] != '\0') {
    const std::string key{newKeyBuffer};
    if (!slot.params.contains(key)) {
      slot.params[key] = MakeInitialValueForKindIndex(newValueKindIndex);
      changed = true;
    }
    newKeyBuffer[0] = '\0';
  }

  if (changed) {
    SyncRawBufferFromSlot(rawState, slot);
  }

  if (ImGui::TreeNode("生 JSON テキスト編集")) {
    if (ImGui::Button("現在値から再読込")) {
      SyncRawBufferFromSlot(rawState, slot);
    }
    ImGui::SameLine();
    if (ImGui::Button("JSON を適用")) {
      nlohmann::json parsed =
          nlohmann::json::parse(rawState.jsonBuffer, nullptr, false);
      if (parsed.is_discarded() || !parsed.is_object()) {
        rawState.parseError =
            "JSON 構文エラー、またはルートが object ではありません";
      } else {
        slot.params = std::move(parsed);
        rawState.parseError.clear();
        changed = true;
      }
    }
    if (!rawState.parseError.empty()) {
      ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s",
                         rawState.parseError.c_str());
    }
    ImGui::InputTextMultiline("##FallbackRawJson", &rawState.jsonBuffer,
                              ImVec2(-FLT_MIN, 160.0f));
    ImGui::TreePop();
  }

  ImGui::PopID();
  outChanged = changed;
}

#else

void DrawFallbackComponentParams(ComponentSlot &, bool &outChanged) {
  outChanged = false;
}

#endif

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsFallbackJson
