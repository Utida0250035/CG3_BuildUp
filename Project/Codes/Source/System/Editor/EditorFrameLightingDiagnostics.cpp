#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrameLightingDiagnostics.h"

#include "ForDebug/ImGui.h"
#include "ForDebug/LightingEffectsDebugImGui.h"
#include "ForDebug/SubsystemFrameProfiler.h"

namespace Atrum::Editor::EditorFrameLightingDiagnostics {

void DrawMenuItems() {
  bool showLightingEffects = Atrum::Debug::IsLightingEffectsDebugVisible();
  if (ImGui::MenuItem("ライティング効果 (Fog/GodRays/影)", nullptr,
                      &showLightingEffects)) {
    Atrum::Debug::SetLightingEffectsDebugVisible(showLightingEffects);
  }
  ImGui::Separator();
  if (ImGui::MenuItem("ライティングパネルにフォーカス")) {
    Atrum::Debug::SetLightingEffectsDebugVisible(true);
    ImGui::SetWindowFocus("ライティング効果 (Fog / GodRays / Shadow)");
  }
  bool subsystemProfiler =
      Atrum::Debug::IsSubsystemFrameProfilerDetailVisible();
  if (ImGui::MenuItem("サブシステム フレームプロファイラ", nullptr,
                      &subsystemProfiler)) {
    Atrum::Debug::SetSubsystemFrameProfilerDetailVisible(subsystemProfiler);
  }
}

void DrawMainMenuBar() {
  if (!ImGui::BeginMainMenuBar()) {
    return;
  }
  if (ImGui::BeginMenu("描画診断")) {
    DrawMenuItems();
    ImGui::EndMenu();
  }
  ImGui::EndMainMenuBar();
}

} // namespace Atrum::Editor::EditorFrameLightingDiagnostics

#endif