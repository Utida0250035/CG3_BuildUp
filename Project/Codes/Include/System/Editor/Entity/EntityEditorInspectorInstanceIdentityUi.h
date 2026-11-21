#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
struct SceneEntityInstance;
struct EntityDefinition;
}

namespace Atrum::EntityEditor::EntityEditorInspectorInstanceIdentityUi {

/// instanceName（ヒエラルキー）と entityName（definition）を独立編集
bool Draw(EntityEditor &editor, SceneEntityInstance &instance,
          EntityDefinition &definition, bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorInspectorInstanceIdentityUi