#pragma once

#include "System/Entity/ComponentUpd.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace Atrum {

constexpr const char *kUpdCmpGroupNames[] = {
    "NONE_GROUPING", "INPUT",      "MOVEMENT",    "PHYSICS",
    "HIT_JUDGE",     "HIT_RESPONSE", "ANIMATION", "PRE_RENDER",
};

constexpr int kUpdCmpGroupNameCount =
    static_cast<int>(sizeof(kUpdCmpGroupNames) / sizeof(kUpdCmpGroupNames[0]));

std::optional<UpdCmpGroup> TryParseUpdCmpGroupName(std::string_view name);

uint64_t UpdCmpGroupToPriority(UpdCmpGroup group);

const char *UpdCmpGroupToName(UpdCmpGroup group);

UpdCmpGroup UpdCmpGroupFromNumericPriority(uint64_t numericPriority);

} // namespace Atrum