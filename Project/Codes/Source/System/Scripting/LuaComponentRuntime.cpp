#include "System/Scripting/LuaComponentRuntime.h"

#include "System/Scripting/LuaSystem.h"

namespace Atrum::Script {

namespace {

LuaSystem *g_activeLuaSystem = nullptr;

} // namespace

void LuaComponentRuntime::SetActive(LuaSystem *luaSystem) {
  g_activeLuaSystem = luaSystem;
}

LuaSystem *LuaComponentRuntime::ActiveSystem() { return g_activeLuaSystem; }

sol::state *LuaComponentRuntime::ActiveState() {
  if (g_activeLuaSystem == nullptr) {
    return nullptr;
  }
  return &g_activeLuaSystem->GetState();
}

} // namespace Atrum::Script