#include "System/Entity/UpdCmpGroupUtil.h"

namespace Atrum {

namespace {

struct UpdCmpGroupEntry {
  const char *name;
  UpdCmpGroup group;
};

constexpr UpdCmpGroupEntry kUpdCmpGroupTable[] = {
    {"NONE_GROUPING", UpdCmpGroup::NONE_GROUPING},
    {"INPUT", UpdCmpGroup::INPUT},
    {"MOVEMENT", UpdCmpGroup::MOVEMENT},
    {"PHYSICS", UpdCmpGroup::PHYSICS},
    {"HIT_JUDGE", UpdCmpGroup::HIT_JUDGE},
    {"HIT_RESPONSE", UpdCmpGroup::HIT_RESPONSE},
    {"ANIMATION", UpdCmpGroup::ANIMATION},
    {"PRE_RENDER", UpdCmpGroup::PRE_RENDER},
};

} // namespace

std::optional<UpdCmpGroup> TryParseUpdCmpGroupName(const std::string_view name) {
  for (const UpdCmpGroupEntry &entry : kUpdCmpGroupTable) {
    if (name == entry.name) {
      return entry.group;
    }
  }
  return std::nullopt;
}

uint64_t UpdCmpGroupToPriority(const UpdCmpGroup group) {
  return static_cast<uint64_t>(group);
}

const char *UpdCmpGroupToName(const UpdCmpGroup group) {
  for (const UpdCmpGroupEntry &entry : kUpdCmpGroupTable) {
    if (entry.group == group) {
      return entry.name;
    }
  }
  return "NONE_GROUPING";
}

UpdCmpGroup UpdCmpGroupFromNumericPriority(const uint64_t numericPriority) {
  return static_cast<UpdCmpGroup>(numericPriority);
}

} // namespace Atrum