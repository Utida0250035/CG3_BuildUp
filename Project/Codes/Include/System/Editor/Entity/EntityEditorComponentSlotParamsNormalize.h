#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace Atrum::EntityEditor {

struct ComponentSlot;

/// 既定 params を基底に JSON の params を上書きマージする（既知型）。未知型は incoming をそのまま返す。
nlohmann::json MergeParamsForRegisteredType(const std::string &typeName,
                                            const nlohmann::json &incoming);

/// slot.params を MergeParamsForRegisteredType で置き換える。
void NormalizeKnownComponentSlotParams(ComponentSlot &slot);

} // namespace Atrum::EntityEditor