#include "ForDebug/LightingEffectsDebugImGui.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "ForDebug/ImGui.h"

namespace Atrum::Debug {

namespace {

bool &LightingEffectsDebugShowFlag() {
  static bool showWindow = false;
  return showWindow;
}

constexpr const char kWindowTitle[] = "ライト・ポスト調整";

} // namespace

const char *LightingEffectsDebugWindowTitle() { return kWindowTitle; }

bool IsLightingEffectsDebugVisible() { return LightingEffectsDebugShowFlag(); }

void SetLightingEffectsDebugVisible(const bool visible) {
  LightingEffectsDebugShowFlag() = visible;
}

void DrawLightingEffectsDebugWindow() {
  bool &showWindow = LightingEffectsDebugShowFlag();
  if (!showWindow) {
    return;
  }

  if (!ImGui::Begin(kWindowTitle, &showWindow)) {
    ImGui::End();
    return;
  }

  ImGui::TextDisabled(
      "VolumetricFog / GodRays / DirectionalShadow は実行経路から除外済みです。"
      "\n方向性ライトは game_scene_lighting.json の directionalLight を参照。");

  ImGui::End();
}

} // namespace Atrum::Debug

#else

namespace Atrum::Debug {

bool IsLightingEffectsDebugVisible() { return false; }

void SetLightingEffectsDebugVisible(const bool /*visible*/) {}

const char *LightingEffectsDebugWindowTitle() { return "ライト・ポスト調整"; }

} // namespace Atrum::Debug

#endif