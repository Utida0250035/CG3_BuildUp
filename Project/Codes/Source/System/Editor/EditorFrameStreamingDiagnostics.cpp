#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrameStreamingDiagnostics.h"

#include "ForDebug/AudioStreamingDebugImGui.h"
#include "ForDebug/ImGui.h"

namespace Atrum::Editor::EditorFrameStreamingDiagnostics {

void DrawMenuItems() {
  bool showStreamingMemory = Atrum::Debug::IsStreamingMemoryDebugVisible();
  if (ImGui::MenuItem("ストリーミングメモリ", nullptr, &showStreamingMemory)) {
    Atrum::Debug::SetStreamingMemoryDebugVisible(showStreamingMemory);
  }
}

} // namespace Atrum::Editor::EditorFrameStreamingDiagnostics

#endif