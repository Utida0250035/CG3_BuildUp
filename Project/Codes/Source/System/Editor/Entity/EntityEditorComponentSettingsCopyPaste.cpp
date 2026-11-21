#include "System/Editor/Entity/EntityEditorComponentSettingsCopyPaste.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#endif

namespace Atrum::EntityEditor {

void CopyComponentSettingsToClipboard(const ComponentSlot &slot, UiState &ui) {
  ComponentSlot snapshot{};
  snapshot.kind = slot.kind;
  snapshot.typeName = slot.typeName;
  snapshot.executionOrder = slot.executionOrder;
  snapshot.params = slot.params;
  ui.clipboardComponentSlot = std::move(snapshot);
}

bool PasteComponentSettingsFromClipboard(ComponentSlot &slot,
                                         const UiState &ui) {
  if (!ui.clipboardComponentSlot.has_value()) {
    return false;
  }
  const ComponentSlot &source = *ui.clipboardComponentSlot;
  if (source.typeName != slot.typeName) {
    return false;
  }
  slot.params = source.params;
  slot.executionOrder = source.executionOrder;
  return true;
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

bool DrawComponentSettingsCopyPasteButtons(EntityEditor &editor,
                                           ComponentSlot &selected) {
  UiState &ui = editor.Ui();

  if (ImGui::Button("設定をコピー")) {
    CopyComponentSettingsToClipboard(selected, ui);
  }
  ImGui::SameLine();
  const bool canPaste = ui.clipboardComponentSlot.has_value() &&
                        ui.clipboardComponentSlot->typeName == selected.typeName;
  if (!canPaste) {
    ImGui::BeginDisabled();
  }
  if (ImGui::Button("設定をペースト")) {
    return PasteComponentSettingsFromClipboard(selected, ui);
  }
  if (!canPaste) {
    ImGui::EndDisabled();
    if (ui.clipboardComponentSlot.has_value()) {
      ImGui::TextDisabled(
          "ペーストには同じ型（%s）を選択してください",
          ui.clipboardComponentSlot->typeName.c_str());
    }
  }
  return false;
}

#else

bool DrawComponentSettingsCopyPasteButtons(EntityEditor &, ComponentSlot &) {
  return false;
}

#endif

} // namespace Atrum::EntityEditor