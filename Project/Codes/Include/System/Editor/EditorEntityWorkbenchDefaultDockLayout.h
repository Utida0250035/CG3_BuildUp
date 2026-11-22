#pragma once

#include <imgui/imgui.h>

namespace Atrum::Editor::EditorEntityWorkbenchDefaultDockLayout {

/// パネル構成が変わったらインクリメント（未適用分は次フレームで DockBuilder 再構築）。
constexpr int kDefaultDockLayoutVersion = 5;

/// ImGui::GetID に渡す DockSpace 識別子。
const char *DockSpaceIdLabel();

/// 既定レイアウトを適用する（version 未達または forceRebuild のときのみ）。
void ApplyIfNeeded(ImGuiID dockSpaceId, ImVec2 workSize, bool forceRebuild);

} // namespace Atrum::Editor::EditorEntityWorkbenchDefaultDockLayout