#include "ForDebug/LightingEffectsDebugImGui.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Renderer/Lighting/LightingEffectsImGuiPanel.h"
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

  AtrumEngine *engine = ::Atrum::EngineInstance();
  if (engine != nullptr) {
    Visual::DrawLightingEffectsImGuiPanel(engine->RefVolumetricFog(),
                                          engine->RefGodRays(),
                                          engine->RefDirectionalShadow());
    engine->FlushDirectionalLightConstantBuffer();
  }

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