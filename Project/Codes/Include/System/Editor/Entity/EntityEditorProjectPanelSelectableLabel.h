#pragma once

#include <string>

namespace Atrum::EntityEditor::ProjectPanelSelectableLabel {

/// プレハブ JSON パスから「プレハブ名（stem）##フルパス」の Selectable 用ラベルを生成する
std::string BuildPrefabSelectableLabel(const std::string &prefabPath);

/// Imported アセットパスから「ファイル名（拡張子付き）##フルパス」の Selectable 用ラベルを生成する
std::string BuildImportedAssetSelectableLabel(const std::string &assetPath);

/// 直前に描画した ImGui アイテムのホバー時に、完全な相対パスをツールチップ表示する
void DrawFullPathHoverTooltip(const std::string &fullPath);

} // namespace Atrum::EntityEditor::ProjectPanelSelectableLabel
