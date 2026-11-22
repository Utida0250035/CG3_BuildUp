#include "System/Editor/EditorImportedAssetDropPolicyUi.h"

#include "System/Editor/Entity/EntityEditor.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#endif

namespace Atrum::Editor::EditorImportedAssetDropPolicyUi {

void DrawDropCollisionPolicyControls(EntityEditor::UiState &ui) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  ImGui::TextDisabled(
      "既定「連番」: 同名ファイルは stem_1, stem_2 … へ自動リネーム");
  const char *policyLabels[] = {"連番 (stem_N)", "上書き",
                                "タイムスタンプ付与", "接頭辞+連番"};
  int policyIndex = static_cast<int>(ui.importCollisionPolicy);
  if (ImGui::Combo("同名衝突", &policyIndex, policyLabels,
                   IM_ARRAYSIZE(policyLabels))) {
    ui.importCollisionPolicy =
        static_cast<EntityEditor::ImportedAssetNameCollisionPolicy>(
            policyIndex);
  }
  if (ui.importCollisionPolicy ==
      EntityEditor::ImportedAssetNameCollisionPolicy::BatchPrefixThenIncrement) {
    ImGui::InputString("接頭辞", ui.importBatchNamePrefix);
  }
#endif
}

} // namespace Atrum::Editor::EditorImportedAssetDropPolicyUi