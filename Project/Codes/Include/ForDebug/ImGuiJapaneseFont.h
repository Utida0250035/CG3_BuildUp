#pragma once

#ifdef USE_IMGUI

struct ImGuiIO;

namespace Atrum::Debug {

/// 日本語グリフを含む ImGui フォントを登録する（失敗時はデフォルトフォント）
void SetupImGuiJapaneseFont(ImGuiIO &io);

} // namespace Atrum::Debug

#endif
