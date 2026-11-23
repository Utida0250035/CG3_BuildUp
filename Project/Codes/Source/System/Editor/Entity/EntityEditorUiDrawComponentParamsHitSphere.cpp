#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsHitSphere.h"

#include "ForDebug/ImGui.h"
#include "Math/Vector3.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <algorithm>
#include <cstdio>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <vector>

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsHitSphere {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace {

struct HitSphereSelectionState {
  const ComponentSlot *lastSlot = nullptr;
  std::set<int> selectedIndices{0};
  int anchorIndex = 0;
  float batchRadius = 0.5f;
  Math::Vector3 batchOffsetDelta{};
};

HitSphereSelectionState &SelectionState() {
  static HitSphereSelectionState state{};
  return state;
}

void SanitizeSelection(HitSphereSelectionState &state, const int sphereCount) {
  if (sphereCount <= 0) {
    state.selectedIndices.clear();
    state.anchorIndex = 0;
    return;
  }

  for (auto it = state.selectedIndices.begin();
       it != state.selectedIndices.end();) {
    if (*it < 0 || *it >= sphereCount) {
      it = state.selectedIndices.erase(it);
    } else {
      ++it;
    }
  }

  if (state.selectedIndices.empty()) {
    state.selectedIndices.insert(0);
    state.anchorIndex = 0;
  }
  state.anchorIndex = std::clamp(state.anchorIndex, 0, sphereCount - 1);
}

void HandleSphereRowClick(HitSphereSelectionState &state, const int index,
                          const int sphereCount) {
  const ImGuiIO &io = ImGui::GetIO();
  if (io.KeyShift) {
    const int lo = std::clamp(std::min(state.anchorIndex, index), 0,
                              sphereCount - 1);
    const int hi = std::clamp(std::max(state.anchorIndex, index), 0,
                              sphereCount - 1);
    if (!io.KeyCtrl) {
      state.selectedIndices.clear();
    }
    for (int i = lo; i <= hi; ++i) {
      state.selectedIndices.insert(i);
    }
    return;
  }

  if (io.KeyCtrl) {
    if (state.selectedIndices.count(index) != 0) {
      if (state.selectedIndices.size() > 1) {
        state.selectedIndices.erase(index);
      }
    } else {
      state.selectedIndices.insert(index);
    }
    state.anchorIndex = index;
    return;
  }

  state.selectedIndices.clear();
  state.selectedIndices.insert(index);
  state.anchorIndex = index;
}

} // namespace

bool TryDrawCmpHitSphereComponentParams(ComponentSlot &slot, bool &outChanged) {
  if (slot.typeName != "CmpHitSphere") {
    return false;
  }

  outChanged = false;
  nlohmann::json &p = slot.params;
  bool changed = false;

  bool active = p.value("isActive", true);
  if (ImGui::Checkbox("isActive", &active)) {
    p["isActive"] = active;
    changed = true;
  }

  if (!p.contains("spheres") || !p.at("spheres").is_array()) {
    p["spheres"] = nlohmann::json::array();
  }
  nlohmann::json &spheres = p.at("spheres");
  if (spheres.empty()) {
    spheres.push_back(
        {{"localOffset", Math::Vector3::Zero()}, {"radius", 0.5f}});
  }

  HitSphereSelectionState &state = SelectionState();
  if (state.lastSlot != &slot) {
    state.lastSlot = &slot;
    state.selectedIndices = {0};
    state.anchorIndex = 0;
  }
  SanitizeSelection(state, static_cast<int>(spheres.size()));

  if (ImGui::Button("球を追加")) {
    const int newIndex = static_cast<int>(spheres.size());
    spheres.push_back(
        {{"localOffset", Math::Vector3::Zero()}, {"radius", 0.5f}});
    state.selectedIndices = {newIndex};
    state.anchorIndex = newIndex;
    changed = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("選択球を複製")) {
    std::set<int> newSelection{};
    const std::vector<int> toDuplicate(state.selectedIndices.begin(),
                                       state.selectedIndices.end());
    for (const int srcIndex : toDuplicate) {
      if (srcIndex < 0 || srcIndex >= static_cast<int>(spheres.size())) {
        continue;
      }
      const int dstIndex = static_cast<int>(spheres.size());
      spheres.push_back(spheres.at(static_cast<size_t>(srcIndex)));
      newSelection.insert(dstIndex);
    }
    if (!newSelection.empty()) {
      state.selectedIndices = std::move(newSelection);
      state.anchorIndex = *state.selectedIndices.begin();
      changed = true;
    }
  }
  ImGui::SameLine();

  const int sphereCountAfterOp = static_cast<int>(spheres.size());
  const bool canRemoveSelected =
      sphereCountAfterOp > 1 && !state.selectedIndices.empty();
  ImGui::BeginDisabled(!canRemoveSelected);
  if (ImGui::Button("選択した球を削除")) {
    // 最低 1 球は残すため、全選択時はインデックス 0 を残して他を削除する
    std::vector<int> eraseIndices{};
    for (auto it = state.selectedIndices.rbegin();
         it != state.selectedIndices.rend(); ++it) {
      const int remaining =
          static_cast<int>(spheres.size()) - static_cast<int>(eraseIndices.size());
      if (remaining <= 1) {
        break;
      }
      eraseIndices.push_back(*it);
    }
    for (const int idx : eraseIndices) {
      if (idx >= 0 && idx < static_cast<int>(spheres.size())) {
        spheres.erase(spheres.begin() + idx);
        changed = true;
      }
    }
    state.selectedIndices = {0};
    state.anchorIndex = 0;
    SanitizeSelection(state, static_cast<int>(spheres.size()));
  }
  ImGui::EndDisabled();

  if (ImGui::Button("全球選択")) {
    state.selectedIndices.clear();
    for (int i = 0; i < static_cast<int>(spheres.size()); ++i) {
      state.selectedIndices.insert(i);
    }
    state.anchorIndex = 0;
  }
  ImGui::SameLine();
  if (ImGui::Button("選択を先頭のみに戻す")) {
    state.selectedIndices = {0};
    state.anchorIndex = 0;
  }
  ImGui::SameLine();
  ImGui::TextDisabled("Ctrl/Shift で複数選択（選択中: %zu / %zu）",
                      state.selectedIndices.size(), spheres.size());

  const int currentSphereCount = static_cast<int>(spheres.size());
  for (int i = 0; i < currentSphereCount; ++i) {
    const nlohmann::json &s = spheres.at(static_cast<size_t>(i));
    const Math::Vector3 off = s.value("localOffset", Math::Vector3::Zero());
    const float rad = s.value("radius", 0.5f);
    char label[160]{};
    std::snprintf(label, sizeof(label),
                  "spheres[%d]  offset(%.2f, %.2f, %.2f)  r=%.2f", i, off.x,
                  off.y, off.z, rad);
    const bool selected = state.selectedIndices.count(i) != 0;
    if (ImGui::Selectable(label, selected)) {
      HandleSphereRowClick(state, i, currentSphereCount);
    }
  }

  if (state.selectedIndices.size() > 1) {
    ImGui::Separator();
    ImGui::Text("選択 %zu 球へ一括編集", state.selectedIndices.size());
    ImGui::DragFloat("一括半径", &state.batchRadius, 0.02f, 0.01f, 64.0f);
    ImGui::SameLine();
    if (ImGui::Button("半径を一括適用")) {
      const float clampedRadius = std::clamp(state.batchRadius, 0.01f, 64.0f);
      for (const int idx : state.selectedIndices) {
        if (idx >= 0 && idx < static_cast<int>(spheres.size())) {
          spheres.at(static_cast<size_t>(idx))["radius"] = clampedRadius;
          changed = true;
        }
      }
    }

    ImGui::DragFloat3("一括オフセット加算", &state.batchOffsetDelta.x, 0.02f);
    ImGui::SameLine();
    if (ImGui::Button("オフセットを加算")) {
      for (const int idx : state.selectedIndices) {
        if (idx >= 0 && idx < static_cast<int>(spheres.size())) {
          nlohmann::json &target = spheres.at(static_cast<size_t>(idx));
          Math::Vector3 current =
              target.value("localOffset", Math::Vector3::Zero());
          current += state.batchOffsetDelta;
          target["localOffset"] = current;
          changed = true;
        }
      }
    }
  }

  ImGui::Separator();
  for (const int idx : state.selectedIndices) {
    if (idx < 0 || idx >= static_cast<int>(spheres.size())) {
      continue;
    }
    nlohmann::json &sphere = spheres.at(static_cast<size_t>(idx));
    const std::string indexSuffix = "[" + std::to_string(idx) + "]";

    Math::Vector3 offset = sphere.value("localOffset", Math::Vector3::Zero());
    const std::string offsetLabel = "localOffset" + indexSuffix;
    if (ImGui::DragFloat3(offsetLabel.c_str(), &offset.x, 0.02f)) {
      sphere["localOffset"] = offset;
      changed = true;
    }
    float radius = sphere.value("radius", 0.5f);
    const std::string radiusLabel = "radius" + indexSuffix;
    if (ImGui::DragFloat(radiusLabel.c_str(), &radius, 0.02f, 0.01f, 64.0f)) {
      sphere["radius"] = radius;
      changed = true;
    }
  }

  outChanged = changed;
  return true;
}

#else

bool TryDrawCmpHitSphereComponentParams(ComponentSlot &, bool &outChanged) {
  outChanged = false;
  return false;
}

#endif

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsHitSphere
