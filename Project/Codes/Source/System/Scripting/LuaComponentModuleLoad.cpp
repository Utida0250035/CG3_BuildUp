#include "System/Scripting/LuaComponentModuleLoad.h"

#include "System/Scripting/LuaComponentRuntime.h"

#include <sol/sol.hpp>
#include <filesystem>
#include <iostream>
#include <string>

namespace Atrum::Script {

namespace {

bool PathExists(const std::string &candidate) {
  if (candidate.empty()) {
    return false;
  }
  namespace fs = std::filesystem;
  std::error_code error{};
  return fs::exists(candidate, error) && !error;
}

} // namespace

std::string ResolveLuaScriptPathForComponent(const std::string &path) {
  if (PathExists(path)) {
    return path;
  }
  const std::string underAssets = "Assets/Scripts/" + path;
  if (PathExists(underAssets)) {
    return underAssets;
  }
  const std::string underComponents = "Assets/Scripts/Components/" + path;
  if (PathExists(underComponents)) {
    return underComponents;
  }
  const std::string underRuntime = "Assets/Scripts/Runtime/" + path;
  if (PathExists(underRuntime)) {
    return underRuntime;
  }
  return path;
}

sol::table LoadLuaComponentModule(const std::string &scriptPath,
                                  std::string *outError) {
  sol::state *lua = LuaComponentRuntime::ActiveState();
  if (lua == nullptr) {
    if (outError != nullptr) {
      *outError = "Lua runtime is not active";
    }
    return sol::lua_nil;
  }

  const std::string resolved = ResolveLuaScriptPathForComponent(scriptPath);
  sol::protected_function_result loadResult =
      lua->safe_script_file(resolved, sol::script_pass_on_error);
  if (!loadResult.valid()) {
    sol::error err = loadResult;
    if (outError != nullptr) {
      *outError = err.what();
    }
    std::cerr << "[LuaComponentModuleLoad] load failed: " << resolved << "\n"
              << err.what() << std::endl;
    return sol::lua_nil;
  }

  sol::object loaded = loadResult;
  if (!loaded.is<sol::table>()) {
    if (outError != nullptr) {
      *outError = "component script must return a table";
    }
    return sol::lua_nil;
  }
  return loaded.as<sol::table>();
}

} // namespace Atrum::Script