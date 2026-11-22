#pragma once

#include <string>

namespace Atrum::EntityEditor {
struct UiState;
}

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsExternalOpen {

/// 画像・モデルパス行の直下に「外部ツールで開く」ボタン
void DrawButton(const std::string &projectRelativePath, UiState &ui);

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsExternalOpen