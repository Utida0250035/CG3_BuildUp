#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrameFrameDeltaDiagnostics.h"

#include "ForDebug/FrameDeltaTimeDebugImGui.h"
#include "ForDebug/ImGui.h"

namespace Atrum::Editor::EditorFrameFrameDeltaDiagnostics {

void DrawMenuItems() {
  bool overlay = Atrum::Debug::IsFrameDeltaTimeOverlayVisible();
  if (ImGui::MenuItem("フレーム時間オーバーレイ", nullptr, &overlay)) {
    Atrum::Debug::SetFrameDeltaTimeOverlayVisible(overlay);
  }
  bool histogram = Atrum::Debug::IsFrameDeltaTimeHistogramVisible();
  if (ImGui::MenuItem("フレーム時間ヒストグラム", nullptr, &histogram)) {
    Atrum::Debug::SetFrameDeltaTimeHistogramVisible(histogram);
  }
}

} // namespace Atrum::Editor::EditorFrameFrameDeltaDiagnostics

#endif