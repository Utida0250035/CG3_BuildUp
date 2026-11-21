#include "System/Editor/Entity/EntityEditorRemoveComponentSlotsBatch.h"

namespace Atrum::EntityEditor::EntityEditorRemoveComponentSlotsBatch {

bool RemoveComponentSlotAtIndex(EntityDefinition &definition, const int index) {
  if (index < 0 || index >= static_cast<int>(definition.components.size())) {
    return false;
  }
  definition.components.erase(definition.components.begin() + index);
  return true;
}

int RemoveAllComponentSlotsOfType(EntityDefinition &definition,
                                  const std::string &typeName) {
  if (typeName.empty()) {
    return 0;
  }
  int removed = 0;
  for (int index = static_cast<int>(definition.components.size()) - 1;
       index >= 0; --index) {
    if (definition.components[static_cast<size_t>(index)].typeName == typeName) {
      definition.components.erase(definition.components.begin() + index);
      ++removed;
    }
  }
  return removed;
}

int RemoveAllComponentSlotsOfKind(EntityDefinition &definition,
                                  const ComponentSlotKind kind) {
  int removed = 0;
  for (int index = static_cast<int>(definition.components.size()) - 1;
       index >= 0; --index) {
    if (definition.components[static_cast<size_t>(index)].kind == kind) {
      definition.components.erase(definition.components.begin() + index);
      ++removed;
    }
  }
  return removed;
}

} // namespace Atrum::EntityEditor::EntityEditorRemoveComponentSlotsBatch