#pragma once

namespace Atrum::EntityEditor {
struct EntityDefinition;
struct SceneEntityInstance;
}

namespace Atrum::EntityEditor::EntityEditorPrefabSaveEntityDefinitionBody {

/// プレハブ JSON 用に instance 名を entityName へ反映した Definition を組み立てる。
EntityDefinition BuildEntityDefinitionForPrefabFile(
    const SceneEntityInstance &instance);

} // namespace Atrum::EntityEditor::EntityEditorPrefabSaveEntityDefinitionBody