#pragma once

#include <vector>

namespace Atrum::EntityEditor {
struct EditorSceneDocument;
struct EntityDefinition;
}

namespace Atrum::EntityEditor::EntityEditorClearAllEntityTags {

void ClearAllTagsOnDefinition(EntityDefinition &definition);

/// 指定インスタンスのタグをすべて解除。処理件数を返す
int ClearAllTagsOnInstanceIndices(EditorSceneDocument &scene,
                                  const std::vector<int> &indices);

} // namespace Atrum::EntityEditor::EntityEditorClearAllEntityTags