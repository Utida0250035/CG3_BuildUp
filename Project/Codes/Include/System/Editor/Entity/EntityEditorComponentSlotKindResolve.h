#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <optional>
#include <string>

namespace Atrum::EntityEditor {

/// 登録済み typeName のみ種別を返す（未登録は nullopt — Draw へ黙って落とさない）。
std::optional<ComponentSlotKind>
TryDefaultKindForRegisteredType(const std::string &typeName);

} // namespace Atrum::EntityEditor