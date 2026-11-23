#include "System/Editor/Entity/EntityEditorInspectorMultiSlotExecutionOrderUi.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorExecutionOrderBatch.h"

#include <imgui/imgui.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace Atrum::EntityEditor::EntityEditorInspectorMultiSlotExecutionOrderUi {

namespace {

struct MultiSlotOrderState {
  const EntityDefinition *lastDocument = nullptr;
  ComponentSlotKind lastKind = ComponentSlotKind::Draw;
  std::set<int> checkedDocIndices{};
  int batchOrder = 0;
};

MultiSlotOrderState &State() {
  static MultiSlotOrderState state{};
  return state;
}

std::vector<int> CollectSortedIndicesForKind(const EntityDefinition &document,
                                             const ComponentSlotKind kind) {
  std::vector<int> indices;
  for (int i = 0; i < static_cast<int>(document.components.size()); ++i) {
    if (document.components[static_cast<size_t>(i)].kind == kind) {
      indices.push_back(i);
    }
  }
  std::sort(indices.begin(), indices.end(),
            [&document](const int a, const int b) {
              const int orderA =
                  document.components[static_cast<size_t>(a)].executionOrder;
              const int orderB =
                  document.components[static_cast<size_t>(b)].executionOrder;
              if (orderA != orderB) {
                return orderA < orderB;
              }
              return a < b;
            });
  return indices;
}

} // namespace

void Draw(EntityEditor &editor, EntityDefinition &document,
          const ComponentSlot &selectedSlot, bool &outChanged) {
  UiState &ui = editor.Ui();

  if (ui.selectedInstanceIndices.size() > 1) {
    const std::string buttonLabel =
        "選択中 " + std::to_string(ui.selectedInstanceIndices.size()) +
        " 体の同型 (" + selectedSlot.typeName + ") へ実行順を適用";
    if (ImGui::Button(buttonLabel.c_str())) {
      EditorSceneDocument &scene = editor.SceneDocument();
      int totalUpdated = 0;
      for (const int instanceIndex : ui.selectedInstanceIndices) {
        if (instanceIndex < 0 ||
            instanceIndex >= static_cast<int>(scene.instances.size())) {
          continue;
        }
        totalUpdated +=
            EntityEditorExecutionOrderBatch::ApplyExecutionOrderToAllSlotsOfType(
                scene.instances[static_cast<size_t>(instanceIndex)].definition,
                selectedSlot.typeName, selectedSlot.executionOrder);
      }
      if (totalUpdated > 0) {
        ui.importStatusMessage = "選択インスタンスの " + selectedSlot.typeName +
                                 " 計 " + std::to_string(totalUpdated) +
                                 " スロットへ実行順 " +
                                 std::to_string(selectedSlot.executionOrder) +
                                 " を適用";
        outChanged = true;
      }
    }
  }

  const std::vector<int> kindIndices =
      CollectSortedIndicesForKind(document, ui.selectedComponentKind);
  if (kindIndices.size() <= 1) {
    return;
  }

  MultiSlotOrderState &state = State();
  if (state.lastDocument != &document ||
      state.lastKind != ui.selectedComponentKind) {
    state.lastDocument = &document;
    state.lastKind = ui.selectedComponentKind;
    state.checkedDocIndices.clear();
    state.batchOrder = selectedSlot.executionOrder;
  }

  for (auto it = state.checkedDocIndices.begin();
       it != state.checkedDocIndices.end();) {
    if (std::find(kindIndices.begin(), kindIndices.end(), *it) ==
        kindIndices.end()) {
      it = state.checkedDocIndices.erase(it);
    } else {
      ++it;
    }
  }

  if (!ImGui::TreeNode("複数スロット実行順一括編集")) {
    return;
  }

  if (ImGui::SmallButton("全スロット選択")) {
    for (const int idx : kindIndices) {
      state.checkedDocIndices.insert(idx);
    }
  }
  ImGui::SameLine();
  if (ImGui::SmallButton("選択解除")) {
    state.checkedDocIndices.clear();
  }
  ImGui::SameLine();
  ImGui::TextDisabled("チェック数: %zu", state.checkedDocIndices.size());

  for (const int docIndex : kindIndices) {
    ComponentSlot &slot = document.components[static_cast<size_t>(docIndex)];
    ImGui::PushID(docIndex);

    bool checked = state.checkedDocIndices.count(docIndex) != 0;
    if (ImGui::Checkbox("##SlotCheck", &checked)) {
      if (checked) {
        state.checkedDocIndices.insert(docIndex);
      } else {
        state.checkedDocIndices.erase(docIndex);
      }
    }
    ImGui::SameLine();

    int order = slot.executionOrder;
    ImGui::SetNextItemWidth(110.0f);
    if (ImGui::InputInt("##InlineExecOrder", &order)) {
      slot.executionOrder = std::max(0, order);
      outChanged = true;
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(slot.typeName.c_str());

    ImGui::PopID();
  }

  ImGui::Separator();
  ImGui::SetNextItemWidth(120.0f);
  if (ImGui::InputInt("一括実行順", &state.batchOrder)) {
    state.batchOrder = std::max(0, state.batchOrder);
  }
  ImGui::SameLine();
  ImGui::BeginDisabled(state.checkedDocIndices.empty());
  if (ImGui::Button("チェック中へ適用")) {
    int applied = 0;
    for (const int docIndex : state.checkedDocIndices) {
      if (docIndex >= 0 &&
          docIndex < static_cast<int>(document.components.size())) {
        document.components[static_cast<size_t>(docIndex)].executionOrder =
            state.batchOrder;
        ++applied;
      }
    }
    if (applied > 0) {
      ui.importStatusMessage = "選択 " + std::to_string(applied) +
                               " スロットの実行順を " +
                               std::to_string(state.batchOrder) + " に設定";
      outChanged = true;
    }
  }
  ImGui::SameLine();
  if (ImGui::SmallButton("-1")) {
    for (const int docIndex : state.checkedDocIndices) {
      if (docIndex >= 0 &&
          docIndex < static_cast<int>(document.components.size())) {
        int &order =
            document.components[static_cast<size_t>(docIndex)].executionOrder;
        order = std::max(0, order - 1);
        outChanged = true;
      }
    }
  }
  ImGui::SameLine();
  if (ImGui::SmallButton("+1")) {
    for (const int docIndex : state.checkedDocIndices) {
      if (docIndex >= 0 &&
          docIndex < static_cast<int>(document.components.size())) {
        ++document.components[static_cast<size_t>(docIndex)].executionOrder;
        outChanged = true;
      }
    }
  }
  ImGui::EndDisabled();

  ImGui::TreePop();
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorMultiSlotExecutionOrderUi
