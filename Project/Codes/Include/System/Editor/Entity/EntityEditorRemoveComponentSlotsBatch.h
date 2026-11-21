#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <string>

namespace Atrum::EntityEditor::EntityEditorRemoveComponentSlotsBatch {

/// 1 スロットを削除。削除後の selectedComponentIndex 調整用に新しいサイズを返す
bool RemoveComponentSlotAtIndex(EntityDefinition &definition, int index);

/// typeName が一致するスロットをすべて削除
int RemoveAllComponentSlotsOfType(EntityDefinition &definition,
                                  const std::string &typeName);

/// Upd/Draw/Data 種別が一致するスロットをすべて削除
int RemoveAllComponentSlotsOfKind(EntityDefinition &definition,
                                  ComponentSlotKind kind);

} // namespace Atrum::EntityEditor::EntityEditorRemoveComponentSlotsBatch