#include "System/Scripting/LuaSystem.h"
#include "System/Scripting/ILuaBinder.h"
#include <iostream>

namespace Atrum::Script {

bool LuaSystem::Initialize() {
  lua_.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string,
                      sol::lib::table);

  return true;
}

void LuaSystem::RegisterBinds() {
  for (auto &binder : binders_) {
    if (!binder)
      continue;
    binder->Register(lua_);
  }
}

bool LuaSystem::ExecuteFile(const std::string &filePath) {
  auto result = lua_.script_file(filePath, sol::script_pass_on_error);

  if (!result.valid()) {
    sol::error err = result;
    std::cerr << "[LuaSystem Error] Failed to execute file:\n"
              << "[Path]: " << filePath << "\n"
              << err.what() << "\n"
              << std::endl;
    return false;
  }

  return true;
}

bool LuaSystem::ExecuteStr(const std::string &code) {
  auto result = lua_.script(code, sol::script_pass_on_error);

  if (!result.valid()) {
    sol::error err = result;
    std::cerr << "[LuaSystem Error] Failed to execute string:\n"
              << err.what() << "\n"
              << std::endl;
    return false;
  }

  return true;
}

sol::state &LuaSystem::GetState() { return lua_; }

} // namespace Atrum::Script