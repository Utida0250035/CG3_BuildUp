#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <string>

namespace Atrum::EntityEditor::EntityEditorExecutionOrderBatch {

/// typeName が一致するスロットへ executionOrder を適用。更新件数を返す
int ApplyExecutionOrderToAllSlotsOfType(EntityDefinition &definition,
                                        const std::string &typeName,
                                        int executionOrder);

/// Upd/Draw/Data 種別が一致するスロットへ executionOrder を適用
int ApplyExecutionOrderToAllSlotsOfKind(EntityDefinition &definition,
                                        ComponentSlotKind kind,
                                        int executionOrder);

/// 種別内を現在の executionOrder 昇順で並べ、startOrder から連番を振る
int RenumberExecutionOrderSequentiallyForKind(EntityDefinition &definition,
                                              ComponentSlotKind kind,
                                              int startOrder = 0);

} // namespace Atrum::EntityEditor::EntityEditorExecutionOrderBatch