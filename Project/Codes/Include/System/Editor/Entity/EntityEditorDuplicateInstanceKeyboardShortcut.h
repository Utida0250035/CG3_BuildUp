#pragma once

namespace Atrum::EntityEditor {

class EntityEditor;

/// Ctrl+D で選択インスタンスを複製（エディタ全体で有効）。
void PollEntityEditorDuplicateInstanceKeyboardShortcut(EntityEditor &editor);

} // namespace Atrum::EntityEditor