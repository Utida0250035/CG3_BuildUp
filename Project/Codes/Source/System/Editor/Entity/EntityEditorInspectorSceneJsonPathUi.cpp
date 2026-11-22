#include "System/Editor/Entity/EntityEditorInspectorSceneJsonPathUi.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneJsonFileEntry.h"

#include <filesystem>
#include <imgui.h>

namespace Atrum::EntityEditor::EntityEditorInspectorSceneJsonPathUi {

bool Draw(EntityEditor &editor, bool &outChanged) {
  outChanged = false;
  UiState &ui = editor.Ui();

  bool pathEdited = ImGui::InputString("シーン JSON パス", ui.jsonFilePath);
  if (pathEdited) {
    ui.selectedScenePath = ui.jsonFilePath;
    outChanged = true;
  }

  ImGui::TextDisabled("保存・プレビューは jsonFilePath を使用します");
  if (!ui.selectedScenePath.empty() &&
      ui.selectedScenePath != ui.jsonFilePath) {
    ImGui::TextDisabled("一覧選択: %s", ui.selectedScenePath.c_str());
    if (ImGui::SmallButton("一覧のパスを反映")) {
      ui.jsonFilePath = ui.selectedScenePath;
      outChanged = true;
      pathEdited = true;
    }
  }

  namespace fs = std::filesystem;
  if (ImGui::Button("パスを検証")) {
    if (ui.jsonFilePath.empty()) {
      ui.importStatusMessage = "シーン JSON パスが空です";
    } else if (!fs::exists(ui.jsonFilePath)) {
      ui.importStatusMessage =
          "ファイルが存在しません: " + ui.jsonFilePath;
    } else if (!Atrum::Editor::Scene::IsEditableSceneJsonPath(ui.jsonFilePath)) {
      ui.importStatusMessage =
          "編集可能シーン配下ではありません: " + ui.jsonFilePath;
    } else {
      ui.importStatusMessage = "シーン JSON パス OK: " + ui.jsonFilePath;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("このパスで開き直す")) {
    if (ui.jsonFilePath.empty()) {
      ui.importStatusMessage = "開くパスが空です";
    } else {
      if (!editor.TryOpenSceneFromPath(ui.jsonFilePath)) {
        if (!ui.pendingSceneSwitchPath.empty()) {
          ui.importStatusMessage = "未保存確認待ち: " + ui.jsonFilePath;
        }
      } else {
        ui.importStatusMessage = "シーンを開きました: " + ui.jsonFilePath;
        outChanged = true;
      }
    }
  }

  return outChanged;
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorSceneJsonPathUi