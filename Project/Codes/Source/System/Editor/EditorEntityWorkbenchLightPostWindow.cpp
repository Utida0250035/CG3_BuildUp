#include "System/Editor/EditorEntityWorkbenchLightPostWindow.h"

#include "ForDebug/LightingEffectsDebugImGui.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include <imgui/imgui.h>
#endif

namespace Atrum::Editor::EditorEntityWorkbenchLightPostWindow {

const char *WindowTitle() { return Atrum::Debug::LightingEffectsDebugWindowTitle(); }

void OpenFromMenu() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  Atrum::Debug::SetLightingEffectsDebugVisible(true);
  ImGui::SetWindowFocus(WindowTitle());
#endif
}

} // namespace Atrum::Editor::EditorEntityWorkbenchLightPostWindow