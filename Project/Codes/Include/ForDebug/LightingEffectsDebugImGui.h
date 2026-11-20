#pragma once

namespace Atrum::Debug {

void SetLightingEffectsDebugVisible(bool visible);
bool IsLightingEffectsDebugVisible();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
void DrawLightingEffectsDebugWindow();
#endif

} // namespace Atrum::Debug