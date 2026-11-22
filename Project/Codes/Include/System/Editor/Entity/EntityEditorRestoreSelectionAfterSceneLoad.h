#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <string>
#include <vector>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorRestoreSelectionAfterSceneLoad {

struct SelectionSnapshot {
  std::vector<std::string> selectedInstanceNames;
  int selectedComponentIndex = 0;
  ComponentSlotKind selectedComponentKind = ComponentSlotKind::Draw;
};

SelectionSnapshot Capture(const EntityEditor &editor);
void Apply(EntityEditor &editor, const SelectionSnapshot &snapshot);

} // namespace Atrum::EntityEditor::EntityEditorRestoreSelectionAfterSceneLoad