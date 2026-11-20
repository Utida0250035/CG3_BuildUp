#include "ForDebug/FrameDeltaTimeDebugImGui.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Engine/EngineAccess.h"
#include "ForDebug/ImGui.h"

#include <algorithm>
#include <cmath>

namespace Atrum::Debug {

namespace {

constexpr int kFrameHistorySize = 120;
constexpr float kSpikeThresholdMs = 33.33f;

bool &OverlayVisibleFlag() {
  static bool visible = true;
  return visible;
}

bool &HistogramWindowFlag() {
  static bool visible = false;
  return visible;
}

float frameHistoryMs_[kFrameHistorySize]{};
int frameHistoryWriteIndex_ = 0;
float exponentialAverageMs_ = 16.0f;

void RecordCurrentFrameSample() {
  FrameDeltaTime *frameDelta = ::Atrum::EngineFrameDeltaTime();
  if (frameDelta == nullptr) {
    return;
  }
  const float deltaMs = frameDelta->GetDeltaTime() * 1000.0f;
  frameHistoryMs_[frameHistoryWriteIndex_] = deltaMs;
  frameHistoryWriteIndex_ = (frameHistoryWriteIndex_ + 1) % kFrameHistorySize;
  constexpr float kEmaBlend = 0.08f;
  exponentialAverageMs_ =
      exponentialAverageMs_ * (1.0f - kEmaBlend) + deltaMs * kEmaBlend;
}

float CurrentFrameMs() {
  FrameDeltaTime *frameDelta = ::Atrum::EngineFrameDeltaTime();
  if (frameDelta == nullptr) {
    return 0.0f;
  }
  return frameDelta->GetDeltaTime() * 1000.0f;
}

void DrawOverlay() {
  if (!OverlayVisibleFlag()) {
    return;
  }

  const float frameMs = CurrentFrameMs();
  const float fps = frameMs > 0.0001f ? 1000.0f / frameMs : 0.0f;

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                           ImGuiWindowFlags_AlwaysAutoResize |
                           ImGuiWindowFlags_NoSavedSettings |
                           ImGuiWindowFlags_NoFocusOnAppearing |
                           ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoInputs;
  ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.55f);
  if (!ImGui::Begin("##FrameDeltaOverlay", nullptr, flags)) {
    ImGui::End();
    return;
  }

  ImGui::Text("FPS: %.1f", fps);
  ImGui::Text("Frame: %.2f ms", frameMs);
  ImGui::Text("Avg: %.2f ms", exponentialAverageMs_);
  if (frameMs >= kSpikeThresholdMs) {
    ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.2f, 1.0f), "スパイク");
  }
  ImGui::End();
}

void DrawHistogramWindow() {
  bool &showWindow = HistogramWindowFlag();
  if (!showWindow) {
    return;
  }

  if (!ImGui::Begin("フレーム時間", &showWindow)) {
    ImGui::End();
    return;
  }

  ImGui::Checkbox("画面オーバーレイ", &OverlayVisibleFlag());
  const float frameMs = CurrentFrameMs();
  const float fps = frameMs > 0.0001f ? 1000.0f / frameMs : 0.0f;
  ImGui::Text("現在: %.2f ms (%.1f FPS)  平均: %.2f ms", frameMs, fps,
              exponentialAverageMs_);

  float maxMs = 20.0f;
  for (const float sample : frameHistoryMs_) {
    maxMs = std::max(maxMs, sample);
  }
  maxMs = std::ceil(maxMs * 1.1f);

  ImGui::PlotHistogram("##FrameMsHist", frameHistoryMs_, kFrameHistorySize, 0,
                       nullptr, 0.0f, maxMs, ImVec2(-1.0f, 96.0f));
  ImGui::TextDisabled("%.0f ms まで表示", static_cast<double>(maxMs));

  ImGui::End();
}

} // namespace

void SetFrameDeltaTimeOverlayVisible(const bool visible) {
  OverlayVisibleFlag() = visible;
}

bool IsFrameDeltaTimeOverlayVisible() { return OverlayVisibleFlag(); }

void SetFrameDeltaTimeHistogramVisible(const bool visible) {
  HistogramWindowFlag() = visible;
}

bool IsFrameDeltaTimeHistogramVisible() { return HistogramWindowFlag(); }

void DrawFrameDeltaTimeDebug() {
  RecordCurrentFrameSample();
  DrawOverlay();
  DrawHistogramWindow();
}

} // namespace Atrum::Debug

#endif