#pragma once

#include "System/Entity/ComponentDraw.h"
#include "System/Entity/ComponentUpd.h"
#include <string_view>

namespace Atrum::Script {

UpdCmpGroup ParseUpdCmpGroupFromString(std::string_view groupName);
DrawCmpGroup ParseDrawCmpGroupFromString(std::string_view groupName);

} // namespace Atrum::Script