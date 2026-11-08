#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Debug {

void SetStreamingMemoryDebugVisible(const bool visible);
bool IsStreamingMemoryDebugVisible();

/// 表示フラグ ON 時にメモリウィンドウを描画
void DrawAudioStreamingMemoryDebugWindow();

} // namespace Atrum::Debug

#endif
