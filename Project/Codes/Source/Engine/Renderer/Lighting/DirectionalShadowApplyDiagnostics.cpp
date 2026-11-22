#include "Engine/Renderer/Lighting/DirectionalShadowApplyDiagnostics.h"

#include "Engine/Resource/LightData.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "ForDebug/DirectionalShadowApplyDebugLog.h"
#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Visual::DirectionalShadowApplyDiagnostics {

namespace {

struct State {
  uint64_t planarDrawCount = 0;
  uint64_t planarSkipCount = 0;
  PlanarDrawSkipReason lastPlanarSkip = PlanarDrawSkipReason::None;
  uint64_t lightDataMergeCount = 0;
  uint32_t lastCastShadowFlag = 0;
};

State &MutableState() {
  static State state{};
  return state;
}

uint32_t ComputeCastShadowFlag(const DirectionalShadowSettings &settings) {
  if (settings.shadeEnabled != 0u || settings.planarEnabled != 0u) {
    return static_cast<uint32_t>(LightFlags::CAST_SHADOW);
  }
  return 0u;
}

} // namespace

void ResetFrameCounters() {
  // 将来フレーム境界でリセットする場合のフック。現状は累積統計を保持。
}

void RecordPlanarGroundShadowDraw() {
  MutableState().planarDrawCount += 1;
  MutableState().lastPlanarSkip = PlanarDrawSkipReason::None;
}

void RecordPlanarGroundShadowSkip(const PlanarDrawSkipReason reason) {
  MutableState().planarSkipCount += 1;
  MutableState().lastPlanarSkip = reason;
}

void RecordDirectionalLightDataMerge(const DirectionalShadowSettings &settings,
                                     const uint32_t directionalLightFlags) {
  MutableState().lightDataMergeCount += 1;
  MutableState().lastCastShadowFlag =
      directionalLightFlags & static_cast<uint32_t>(LightFlags::CAST_SHADOW);
  (void)settings;
}

ApplySnapshot GetSnapshot() {
  const State &state = MutableState();
  ApplySnapshot out{};
  out.shadeEnabled = 0;
  out.planarEnabled = 0;
  out.castShadowFlag = state.lastCastShadowFlag;
  out.planarDrawCount = state.planarDrawCount;
  out.planarSkipCount = state.planarSkipCount;
  out.lastPlanarSkip = state.lastPlanarSkip;
  out.lightDataMergeCount = state.lightDataMergeCount;
  return out;
}

const char *PlanarDrawSkipReasonLabel(const PlanarDrawSkipReason reason) {
  switch (reason) {
  case PlanarDrawSkipReason::NullEngine:
    return "engine==nullptr";
  case PlanarDrawSkipReason::PlanarDisabled:
    return "planarEnabled==0";
  case PlanarDrawSkipReason::NonPositiveRadius:
    return "footprintRadius<=0";
  case PlanarDrawSkipReason::None:
  default:
    return "(none)";
  }
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

void DrawApplyStateOverlay(const DirectionalShadowSettings &settings) {
  const ApplySnapshot snapshot = GetSnapshot();
  const uint32_t castShadow = ComputeCastShadowFlag(settings);

  if (ImGui::TreeNode("適用状態 / 診断")) {
    const bool shadeOn = settings.shadeEnabled != 0u;
    const bool planarOn = settings.planarEnabled != 0u;
    const bool castOn = castShadow != 0u;

    ImGui::Text("シェード (PS): %s", shadeOn ? "ON" : "OFF");
    ImGui::Text("平面影ディスク: %s", planarOn ? "ON" : "OFF");
    ImGui::TextColored(castOn ? ImVec4{0.4f, 1.0f, 0.5f, 1.0f}
                              : ImVec4{1.0f, 0.45f, 0.4f, 1.0f},
                      "LightFlags::CAST_SHADOW: %s", castOn ? "ON" : "OFF");

    ImGui::Separator();
    ImGui::Text("平面影 draw: %llu  skip: %llu  merge: %llu",
                static_cast<unsigned long long>(snapshot.planarDrawCount),
                static_cast<unsigned long long>(snapshot.planarSkipCount),
                static_cast<unsigned long long>(snapshot.lightDataMergeCount));
    if (snapshot.planarSkipCount > 0) {
      ImGui::Text("直近 skip 理由: %s",
                  PlanarDrawSkipReasonLabel(snapshot.lastPlanarSkip));
    }

    if (ImGui::Button("適用状態をログへ")) {
      Atrum::Debug::AppendDirectionalShadowApplySnapshotToLog(settings,
                                                               snapshot);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(LogFile)");

    ImGui::TreePop();
  }
}

#endif

} // namespace Atrum::Visual::DirectionalShadowApplyDiagnostics