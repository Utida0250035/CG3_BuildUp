#include "System/Editor/Prefab/PrefabAssetOutputCollisionPolicyUi.h"

#include "System/Editor/Entity/EntityEditor.h"

#include <cstring>
#include <imgui/imgui.h>

namespace Atrum::Editor::Prefab::PrefabAssetOutputCollisionPolicyUi {

void DrawPrefabOutputCollisionPolicyControls(EntityEditor::UiState &ui) {
  ImGui::TextDisabled(
      "新規出力の同名衝突: 既定は stem_1, stem_2 …（上書きは別ボタンでも可）");
  const char *policyLabels[] = {"連番 (stem_N)", "上書き",
                                "タイムスタンプ付与", "接頭辞+連番"};
  int policyIndex = static_cast<int>(ui.prefabOutputCollisionPolicy);
  if (ImGui::Combo("プレハブ出力の同名衝突", &policyIndex, policyLabels,
                   IM_ARRAYSIZE(policyLabels))) {
    ui.prefabOutputCollisionPolicy =
        static_cast<EntityEditor::PrefabAssetNameCollisionPolicy>(policyIndex);
  }
  if (ui.prefabOutputCollisionPolicy ==
      EntityEditor::PrefabAssetNameCollisionPolicy::BatchPrefixThenIncrement) {
    char buffer[128]{};
    const size_t copyCount =
        ui.prefabOutputBatchNamePrefix.size() < sizeof(buffer) - 1
            ? ui.prefabOutputBatchNamePrefix.size()
            : sizeof(buffer) - 1;
    std::memcpy(buffer, ui.prefabOutputBatchNamePrefix.data(), copyCount);
    buffer[copyCount] = '\0';
    if (ImGui::InputText("接頭辞", buffer, sizeof(buffer))) {
      ui.prefabOutputBatchNamePrefix = buffer;
    }
  }
}

} // namespace Atrum::Editor::Prefab::PrefabAssetOutputCollisionPolicyUi