#include "System/Editor/Entity/EntityEditorClearPrefabSelectionOnSceneOpen.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor::EntityEditorClearPrefabSelectionOnSceneOpen {

void OnSceneDocumentOpened(EntityEditor::UiState &ui) {
  ui.selectedPrefabPath.clear();
}

} // namespace Atrum::EntityEditor::EntityEditorClearPrefabSelectionOnSceneOpen