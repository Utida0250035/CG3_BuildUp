#include "System/Entity/Components/CmpLuaUpd.h"

#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Scripting/LuaCmpGroupParse.h"
#include "System/Scripting/LuaComponentEntityContext.h"
#include "System/Scripting/LuaComponentModuleLoad.h"
#include "System/Scripting/LuaComponentRuntime.h"

#include <sol/sol.hpp>
#include <iostream>
#include <string>

namespace Atrum {

void CmpLuaUpd::AddOrUpdateBinding(const std::string &scriptPath,
                                   const std::string_view updCmpGroupName) {
  if (scriptPath.empty()) {
    return;
  }

  for (Binding &existing : bindings_) {
    if (existing.scriptPath == scriptPath) {
      existing.group = Script::ParseUpdCmpGroupFromString(updCmpGroupName);
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
  binding.group = Script::ParseUpdCmpGroupFromString(updCmpGroupName);
  bindings_.push_back(std::move(binding));
  if (bindings_.size() == 1) {
    primaryGroup_ = bindings_.front().group;
  }
}

void CmpLuaUpd::LoadBindingModule(Binding &binding) {
  if (binding.moduleLoaded) {
    return;
  }
  std::string error{};
  binding.module =
      Script::LoadLuaComponentModule(binding.scriptPath, &error);
  binding.moduleLoaded = true;
  if (!binding.module.valid() || binding.module == sol::lua_nil) {
    std::cerr << "[CmpLuaUpd] module load failed: " << binding.scriptPath;
    if (!error.empty()) {
      std::cerr << " — " << error;
    }
    std::cerr << std::endl;
  }
}

void CmpLuaUpd::InvokeResolveForBinding(Binding &binding) {
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
    std::cerr << "[CmpLuaUpd] onResolve error (" << binding.scriptPath
              << "): " << err.what() << std::endl;
  }
}

void CmpLuaUpd::InvokeFinalizeForBinding(const Binding &binding) const {
  if (!binding.module.valid() || binding.module == sol::lua_nil) {
    return;
  }
  sol::optional<sol::protected_function> onExit = binding.module["onExit"];
  if (!onExit) {
    return;
  }
  sol::state *lua = Script::LuaComponentRuntime::ActiveState();
  if (lua == nullptr) {
    return;
  }
  const sol::protected_function_result result = (*onExit)();
  if (!result.valid()) {
    sol::error err = result;
    std::cerr << "[CmpLuaUpd] onExit error (" << binding.scriptPath
              << "): " << err.what() << std::endl;
  }
}

void CmpLuaUpd::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  for (Binding &binding : bindings_) {
    LoadBindingModule(binding);
    if (binding.module.valid() && binding.module != sol::lua_nil) {
      const sol::optional<std::string> moduleGroup =
          binding.module["updCmpGroup"];
      if (moduleGroup && !moduleGroup->empty()) {
        binding.group =
            Script::ParseUpdCmpGroupFromString(*moduleGroup);
        if (bindings_.front().scriptPath == binding.scriptPath) {
          primaryGroup_ = binding.group;
        }
      }
    }
    InvokeResolveForBinding(binding);
  }
}

void CmpLuaUpd::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  const float deltaTime = frameDeltaTime_->GetDeltaTime();
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
    sol::optional<sol::protected_function> onUpdate = binding.module["onUpdate"];
    if (!onUpdate) {
      continue;
    }
    const sol::protected_function_result result = (*onUpdate)(deltaTime, ctx);
    if (!result.valid()) {
      sol::error err = result;
      std::cerr << "[CmpLuaUpd] onUpdate error (" << binding.scriptPath
                << "): " << err.what() << std::endl;
    }
  }
}

void CmpLuaUpd::Finalize() {
  for (const Binding &binding : bindings_) {
    InvokeFinalizeForBinding(binding);
  }
  bindings_.clear();
}

} // namespace Atrum