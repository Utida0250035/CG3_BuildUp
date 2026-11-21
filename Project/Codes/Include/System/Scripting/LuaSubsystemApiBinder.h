#pragma once

#include "System/Scripting/ILuaBinder.h"
#include "System/Scripting/LuaEntityApiBinder.h"

namespace Atrum::Script {

/// Command / Audio / Storage / Registry 系クラスを Lua から利用する API
class LuaSubsystemApiBinder final : public ILuaBinder {
public:
  explicit LuaSubsystemApiBinder(LuaEntityApiContext context);

  void Register(sol::state &lua) override;

private:
  LuaEntityApiContext context_{};
};

} // namespace Atrum::Script