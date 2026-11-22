#pragma once

#include "System/Editor/Entity/EntityEditorSceneObjectAddPreset.h"

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorAddEmptyInstanceFromSelection {

/// プリセットまたは選択定義を複製してルートに追加する。
void AppendInstance(EntityEditor &editor,
                    SceneObjectAddPreset preset = SceneObjectAddPreset::CopySelection);

} // namespace Atrum::EntityEditor::EntityEditorAddEmptyInstanceFromSelection