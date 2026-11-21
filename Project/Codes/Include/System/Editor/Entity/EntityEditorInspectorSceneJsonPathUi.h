#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorInspectorSceneJsonPathUi {

/// インスペクタ先頭: 編集中シーン JSON パスの自由編集・検証・再読込
bool Draw(EntityEditor &editor, bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorInspectorSceneJsonPathUi