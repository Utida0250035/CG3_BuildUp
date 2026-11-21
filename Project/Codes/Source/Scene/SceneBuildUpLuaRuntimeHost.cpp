#include "Scene/SceneBuildUpLuaRuntimeHost.h"

#include "System/Scripting/LuaComponentRuntime.h"
#include "System/Scripting/LuaEntityApiBinder.h"
#include "System/Scripting/LuaSubsystemApiBinder.h"
#include "System/Scripting/LuaEntitySpawnFromJson.h"
#include "System/Scripting/LuaSystem.h"

#include <filesystem>
#include <iostream>
#include <utility>

namespace Atrum {

namespace {

constexpr const char *kBootstrapLuaPath =
    "Assets/Scripts/Runtime/bootstrap.lua";

bool ScriptPathExists(const std::string &path) {
  namespace fs = std::filesystem;
  std::error_code error{};
  return fs::exists(path, error) && !error;
}

} // namespace

void SceneBuildUpLuaRuntimeHost::OnEnterScene(
    std::vector<std::unique_ptr<Entity>> *entities) {
  OnExitScene();
  if (entities == nullptr) {
    return;
  }

  entities_ = entities;
  lua_ = std::make_unique<Script::LuaSystem>();
  if (!lua_->Initialize()) {
    std::cerr << "[SceneBuildUpLuaRuntimeHost] LuaSystem initialize failed"
              << std::endl;
    lua_.reset();
    entities_ = nullptr;
    return;
  }

  Script::LuaEntityApiContext context{};
  context.entities = entities_;
  context.tickCallbacks = &tickCallbacks_;
  lua_->AddBinder<Script::LuaEntityApiBinder>(context);
  lua_->AddBinder<Script::LuaSubsystemApiBinder>(context);
  lua_->RegisterBinds();
  Script::LuaComponentRuntime::SetActive(lua_.get());

  active_ = true;

  if (ScriptPathExists(kBootstrapLuaPath)) {
    if (!lua_->ExecuteFile(kBootstrapLuaPath)) {
      std::cerr << "[SceneBuildUpLuaRuntimeHost] bootstrap failed: "
                << kBootstrapLuaPath << std::endl;
    }
  }
}

void SceneBuildUpLuaRuntimeHost::OnUpdate() {
  if (!active_ || lua_ == nullptr) {
    return;
  }

  for (sol::protected_function &onTick : tickCallbacks_) {
    if (!onTick.valid()) {
      continue;
    }
    sol::protected_function_result tickResult = onTick();
    if (!tickResult.valid()) {
      sol::error err = tickResult;
      std::cerr << "[SceneBuildUpLuaRuntimeHost] onTick error: " << err.what()
                << std::endl;
    }
  }
}

void SceneBuildUpLuaRuntimeHost::OnExitScene() {
  Script::LuaComponentRuntime::SetActive(nullptr);
  tickCallbacks_.clear();
  entities_ = nullptr;
  active_ = false;
  lua_.reset();
}

} // namespace Atrum