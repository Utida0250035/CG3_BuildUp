#include "System/Editor/Entity/EntityEditorDuplicateInstanceKeyboardShortcut.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include <imgui/imgui.h>
#endif

namespace Atrum::EntityEditor {

void PollEntityEditorDuplicateInstanceKeyboardShortcut(EntityEditor &editor) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (editor.SceneDocument().instances.empty()) {
    return;
  }
  if (editor.Ui().interactionMode == EditorInteractionMode::Play) {
    return;
  }
  const ImGuiInputFlags shortcutFlags = ImGuiInputFlags_RouteGlobal;
  if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D, shortcutFlags)) {
    editor.DuplicateSelectedInstance();
  }
#endif
}

} // namespace Atrum::EntityEditor