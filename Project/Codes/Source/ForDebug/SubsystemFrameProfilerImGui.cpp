#include "ForDebug/SubsystemFrameProfiler.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "ForDebug/ImGui.h"

#include <algorithm>
#include <string>

namespace Atrum::Debug {

namespace {

bool &OverlayVisibleFlag() {
  static bool visible = true;
  return visible;
}

bool &DetailWindowFlag() {
  static bool visible = false;
  return visible;
}

void DrawBars(const float maxMs) {
  const float safeMax = std::max(maxMs, 0.01f);
  for (std::size_t i = 0; i < static_cast<std::size_t>(SubsystemFrameId::Count);
       ++i) {
    const auto id = static_cast<SubsystemFrameId>(i);
    const float ms = SubsystemFrameLastSampleMs(id);
    if (ms <= 0.0001f) {
      continue;
    }
    const float fraction = std::min(ms / safeMax, 1.0f);
    ImGui::Text("%s", SubsystemFrameIdLabel(id));
    ImGui::SameLine(150.0f);
    ImGui::ProgressBar(fraction, ImVec2(-1.0f, 0.0f),
                       (std::to_string(ms) + " ms").c_str());
  }
}

void DrawOverlay() {
  if (!OverlayVisibleFlag()) {
    return;
  }

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                           ImGuiWindowFlags_AlwaysAutoResize |
                           ImGuiWindowFlags_NoSavedSettings |
                           ImGuiWindowFlags_NoFocusOnAppearing |
                           ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoInputs;
  ImGui::SetNextWindowPos(ImVec2(12.0f, 72.0f), ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.55f);
  if (!ImGui::Begin("##SubsystemFrameProfilerOverlay", nullptr, flags)) {
    ImGui::End();
    return;
  }

  float maxMs = 0.0f;
  for (std::size_t i = 0; i < static_cast<std::size_t>(SubsystemFrameId::Count);
       ++i) {
    maxMs = std::max(
        maxMs, SubsystemFrameLastSampleMs(static_cast<SubsystemFrameId>(i)));
  }
  ImGui::Text("サブシステム (直近)");
  DrawBars(maxMs);
  ImGui::End();
}

void DrawDetailWindow() {
  bool &showWindow = DetailWindowFlag();
  if (!showWindow) {
    return;
  }

  if (!ImGui::Begin("サブシステム フレームプロファイラ", &showWindow)) {
    ImGui::End();
    return;
  }

  ImGui::Checkbox("画面オーバーレイ", &OverlayVisibleFlag());
  float maxMs = 0.0f;
  for (std::size_t i = 0; i < static_cast<std::size_t>(SubsystemFrameId::Count);
       ++i) {
    maxMs = std::max(
        maxMs, SubsystemFrameLastSampleMs(static_cast<SubsystemFrameId>(i)));
  }
  DrawBars(maxMs);
  ImGui::TextDisabled("DEVELOPMENT ビルドのみ。区間は重複しないよう GameOrder / Scene / Engine に配置。");
  ImGui::End();
}

} // namespace

void SetSubsystemFrameProfilerDetailVisible(const bool visible) {
  DetailWindowFlag() = visible;
}

bool IsSubsystemFrameProfilerDetailVisible() { return DetailWindowFlag(); }

void DrawSubsystemFrameProfilerDebug() {
  DrawOverlay();
  DrawDetailWindow();
}

} // namespace Atrum::Debug

#endif