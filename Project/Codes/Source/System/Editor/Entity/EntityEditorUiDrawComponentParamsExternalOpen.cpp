#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsExternalOpen.h"

#include "System/Editor/EditorExternalAssetOpen.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <imgui/imgui.h>

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsExternalOpen {

void DrawButton(const std::string &projectRelativePath, UiState &ui) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  const bool canTry =
      Atrum::Editor::IsExternalAssetEditorTargetExtension(projectRelativePath);
  ImGui::BeginDisabled(projectRelativePath.empty() || !canTry);
  if (ImGui::Button("外部ツールで開く")) {
    std::string error;
    if (Atrum::Editor::TryOpenExternalAssetWithDefaultApp(projectRelativePath,
                                                          error)) {
      ui.importStatusMessage = "外部ツールで開きました: " + projectRelativePath;
    } else {
      ui.importStatusMessage = error.empty() ? "外部ツールで開けませんでした" : error;
    }
  }
  ImGui::EndDisabled();
  if (!projectRelativePath.empty() && !canTry) {
    ImGui::TextDisabled("png / jpg / obj / gltf / glb のみ対応");
  }
#endif
}

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsExternalOpen