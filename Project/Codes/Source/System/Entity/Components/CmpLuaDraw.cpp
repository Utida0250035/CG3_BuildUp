#include "System/Entity/Components/CmpLuaDraw.h"

#include "System/Entity/Entity.h"
#include "System/Scripting/LuaCmpGroupParse.h"
#include "System/Scripting/LuaComponentEntityContext.h"
#include "System/Scripting/LuaComponentModuleLoad.h"
#include "System/Scripting/LuaComponentRuntime.h"

#include <sol/sol.hpp>
#include <iostream>

namespace Atrum {

void CmpLuaDraw::AddOrUpdateBinding(const std::string &scriptPath,
                                    const std::string_view drawCmpGroupName) {
  if (scriptPath.empty()) {
    return;
  }

  for (Binding &existing : bindings_) {
    if (existing.scriptPath == scriptPath) {
      existing.group = Script::ParseDrawCmpGroupFromString(drawCmpGroupName);
      existing.moduleLoaded = false;
      existing.module = sol::lua_nil;
      if (bindings_.front().scriptPath == scriptPath) {
        primaryGroup_ = existing.group;
      }
      return;
    }
  }

  Binding binding{};
  binding.scriptPath = scriptPath;
  binding.group = Script::ParseDrawCmpGroupFromString(drawCmpGroupName);
  bindings_.push_back(std::move(binding));
  if (bindings_.size() == 1) {
    primaryGroup_ = bindings_.front().group;
  }
}

void CmpLuaDraw::LoadBindingModule(Binding &binding) {
  if (binding.moduleLoaded) {
    return;
  }
  std::string error{};
  binding.module =
      Script::LoadLuaComponentModule(binding.scriptPath, &error);
  binding.moduleLoaded = true;
  if (!binding.module.valid() || binding.module == sol::lua_nil) {
    std::cerr << "[CmpLuaDraw] module load failed: " << binding.scriptPath;
    if (!error.empty()) {
      std::cerr << " — " << error;
    }
    std::cerr << std::endl;
  }
}

void CmpLuaDraw::InvokeResolveForBinding(Binding &binding) {
  if (!binding.module.valid() || binding.module == sol::lua_nil) {
    return;
  }
  sol::optional<sol::protected_function> onResolve = binding.module["onResolve"];
  if (!onResolve) {
    return;
  }
  sol::state *lua = Script::LuaComponentRuntime::ActiveState();
  if (lua == nullptr) {
    return;
  }
  sol::table ctx = Script::MakeLuaComponentEntityContext(*lua, RefOwner());
  const sol::protected_function_result result = (*onResolve)(ctx);
  if (!result.valid()) {
    sol::error err = result;
    std::cerr << "[CmpLuaDraw] onResolve error (" << binding.scriptPath
              << "): " << err.what() << std::endl;
  }
}

void CmpLuaDraw::InvokeFinalizeForBinding(const Binding &binding) const {
  if (!binding.module.valid() || binding.module == sol::lua_nil) {
    return;
  }
  sol::optional<sol::protected_function> onExit = binding.module["onExit"];
  if (!onExit) {
    return;
  }
  const sol::protected_function_result result = (*onExit)();
  if (!result.valid()) {
    sol::error err = result;
    std::cerr << "[CmpLuaDraw] onExit error (" << binding.scriptPath
              << "): " << err.what() << std::endl;
  }
}

void CmpLuaDraw::ResolveDependence() {
  for (Binding &binding : bindings_) {
    LoadBindingModule(binding);
    InvokeResolveForBinding(binding);
  }
}

void CmpLuaDraw::Draw() {
  sol::state *lua = Script::LuaComponentRuntime::ActiveState();
  if (lua == nullptr) {
    return;
  }
  sol::table ctx = Script::MakeLuaComponentEntityContext(*lua, RefOwner());

  for (Binding &binding : bindings_) {
    if (!binding.moduleLoaded) {
      LoadBindingModule(binding);
    }
    if (!binding.module.valid() || binding.module == sol::lua_nil) {
      continue;
    }
    sol::optional<sol::protected_function> onDraw = binding.module["onDraw"];
    if (!onDraw) {
      continue;
    }
    const sol::protected_function_result result = (*onDraw)(ctx);
    if (!result.valid()) {
      sol::error err = result;
      std::cerr << "[CmpLuaDraw] onDraw error (" << binding.scriptPath
                << "): " << err.what() << std::endl;
    }
  }
}

void CmpLuaDraw::Finalize() {
  for (const Binding &binding : bindings_) {
    InvokeFinalizeForBinding(binding);
  }
  bindings_.clear();
}

} // namespace Atrum