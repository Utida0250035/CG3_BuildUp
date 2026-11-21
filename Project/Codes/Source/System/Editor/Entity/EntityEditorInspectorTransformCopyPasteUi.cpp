#include "System/Editor/Entity/EntityEditorInspectorTransformCopyPasteUi.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorTransformCopyPaste.h"

#include <imgui.h>

namespace Atrum::EntityEditor::EntityEditorInspectorTransformCopyPasteUi {

bool Draw(EntityEditor &editor, Math::TransformLH &initialTransform,
          bool &outChanged) {
  outChanged = false;
  UiState &ui = editor.Ui();

  if (ImGui::Button("トランスフォームをコピー")) {
    EntityEditorTransformCopyPaste::CopyInitialTransformToClipboard(
        ui, initialTransform);
    ui.importStatusMessage = "初期トランスフォームをコピーしました";
  }
  ImGui::SameLine();
  ImGui::BeginDisabled(!ui.clipboardInitialTransform.has_value());
  if (ImGui::Button("貼り付け")) {
    if (EntityEditorTransformCopyPaste::PasteInitialTransformFromClipboard(
            ui, initialTransform)) {
      outChanged = true;
      ui.importStatusMessage = "初期トランスフォームを貼り付け";
    }
  }
  ImGui::EndDisabled();

  const size_t selectionCount = ui.selectedInstanceIndices.size();
  if (selectionCount > 1) {
    ImGui::BeginDisabled(!ui.clipboardInitialTransform.has_value());
    if (ImGui::Button("選択へ一括適用")) {
      const int applied =
          EntityEditorTransformCopyPaste::ApplyClipboardTransformToInstanceIndices(
              editor, ui.selectedInstanceIndices);
      if (applied > 0) {
        outChanged = true;
        ui.importStatusMessage =
            "初期トランスフォームを " + std::to_string(applied) + " 体へ適用";
      }
    }
    ImGui::EndDisabled();
    ImGui::TextDisabled("Ctrl+クリック複数選択時に一括適用");
  }

  return outChanged;
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorTransformCopyPasteUi