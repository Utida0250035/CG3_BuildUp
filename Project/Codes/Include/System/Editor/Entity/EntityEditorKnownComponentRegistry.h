#pragma once

#include <span>
#include <string>
#include <string_view>

namespace Atrum::EntityEditor::EntityEditorKnownComponentRegistry {

/// エディタ登録コンポーネント（Definition / UI / Runtime プレビュー経路の正本）。
bool Contains(const std::string &typeName);

/// 登録済みコンポーネント型名の一覧（Lua からの列挙用）。
std::span<const std::string_view> TypeNames();

/// component_split_guidelines.md の分割例 Draw 型が登録表に含まれるか（開発時 assert 用）。
bool GuidelineDrawExamplesAreRegistered();

} // namespace Atrum::EntityEditor::EntityEditorKnownComponentRegistry