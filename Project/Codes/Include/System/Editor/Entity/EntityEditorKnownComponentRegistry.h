#pragma once

#include <string>
#include <string_view>

namespace Atrum::EntityEditor::EntityEditorKnownComponentRegistry {

/// エディタ登録コンポーネント（Definition / UI / Runtime プレビュー経路の正本）。
bool Contains(const std::string &typeName);

/// component_split_guidelines.md の分割例 Draw 型が登録表に含まれるか（開発時 assert 用）。
bool GuidelineDrawExamplesAreRegistered();

} // namespace Atrum::EntityEditor::EntityEditorKnownComponentRegistry