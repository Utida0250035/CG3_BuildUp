#pragma once

namespace Atrum::Debug {

void SetLightingEffectsDebugVisible(bool visible);
bool IsLightingEffectsDebugVisible();

/// ImGui::Begin タイトル（メニューの SetWindowFocus と一致させる）。
const char *LightingEffectsDebugWindowTitle();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
void DrawLightingEffectsDebugWindow();
#endif

} // namespace Atrum::Debug