#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsCmpScriptUpd.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Entity/UpdCmpGroupUtil.h"

#include <algorithm>
#include <cstring>

#include <imgui.h>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsCmpScriptUpd {

namespace {

void EnsureParamsObject(ComponentSlot &slot) {
  if (!slot.params.is_object()) {
    slot.params = nlohmann::json::object();
  }
}

bool DrawStringParam(const char *label, nlohmann::json &params, const char *key,
                     const std::string &fallback) {
  std::string value = params.value(key, fallback);
  char buffer[512]{};
  const size_t copyLen = std::min(value.size(), sizeof(buffer) - 1);
  std::memcpy(buffer, value.c_str(), copyLen);

  const bool changed = ImGui::InputText(label, buffer, sizeof(buffer));
  if (changed) {
    params[key] = std::string(buffer);
  }
  return changed;
}

} // namespace

bool TryDraw(ComponentSlot &slot, bool &outChanged) {
  if (slot.typeName != "CmpScriptUpd") {
    return false;
  }

  EnsureParamsObject(slot);

  bool changed = false;

  const std::string mode =
      slot.params.value("updGroupPriorityMode", std::string("enum"));
  const bool useNumeric = (mode == "numeric");

  int modeIndex = useNumeric ? 1 : 0;
  const char *const modes[] = {"UpdCmpGroup 列挙", "数値を直接指定"};
  if (ImGui::Combo("更新順（優先度）の指定", &modeIndex, modes, 2)) {
    slot.params["updGroupPriorityMode"] = (modeIndex == 1) ? "numeric" : "enum";
    changed = true;
  }

  if (modeIndex == 0) {
  int groupIndex = 2;
  const std::string currentName =
      slot.params.value("updCmpGroup", std::string("MOVEMENT"));
  for (int i = 0; i < kUpdCmpGroupNameCount; ++i) {
    if (currentName == kUpdCmpGroupNames[i]) {
      groupIndex = i;
      break;
    }
  }

    if (ImGui::Combo("UpdCmpGroup", &groupIndex, kUpdCmpGroupNames,
                     kUpdCmpGroupNameCount)) {
      slot.params["updCmpGroup"] = kUpdCmpGroupNames[groupIndex];
      changed = true;
    }

    const auto parsed = TryParseUpdCmpGroupName(kUpdCmpGroupNames[groupIndex]);
    if (parsed.has_value()) {
      slot.params["updGroupPriority"] = UpdCmpGroupToPriority(*parsed);
    }
  } else {
    int numeric = static_cast<int>(
        slot.params.value("updGroupPriority",
                          static_cast<uint64_t>(UpdCmpGroup::MOVEMENT)));
    if (ImGui::InputInt("優先度（数値）", &numeric)) {
      numeric = std::max(0, numeric);
      slot.params["updGroupPriority"] = static_cast<uint64_t>(numeric);
      slot.params["updCmpGroup"] =
          UpdCmpGroupToName(UpdCmpGroupFromNumericPriority(
              static_cast<uint64_t>(numeric)));
      changed = true;
    }
    ImGui::TextDisabled(
        "UpdGroup() は static_cast した列挙値として扱われます（%llu）",
        static_cast<unsigned long long>(slot.params.value(
            "updGroupPriority",
            static_cast<uint64_t>(UpdCmpGroup::MOVEMENT))));
  }

  changed |= DrawStringParam(
      "scriptPath (Assets/Scripts からの相対)", slot.params, "scriptPath", "");

  ImGui::TextDisabled(
      "例: Components/Upd/MyLogic.lua（スロットの executionOrder が 0 より大きいと優先度を上書き）");

  outChanged |= changed;
  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsCmpScriptUpd