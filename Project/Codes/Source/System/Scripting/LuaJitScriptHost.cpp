#include "System/Scripting/LuaJitScriptHost.h"

#include "System/Entity/Entity.h"
#include "System/Scripting/LuaSystem.h"

#include <ForDebug/Log.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace Atrum::Script {

namespace {

std::string MakeInstanceKey(const uint64_t entityKey,
                            const std::string &scriptPath) {
  return std::to_string(entityKey) + "|" + scriptPath;
}

struct LoadedScriptInstance {
  sol::table module{};
  bool valid = false;
};

const char *HookFunctionName(LuaScriptHook hook) {
  switch (hook) {
  case LuaScriptHook::Resolve:
    return "onResolve";
  case LuaScriptHook::Update:
    return "onUpdate";
  }
  return "";
}

} // namespace

struct LuaJitScriptHost::Impl {
  std::unordered_map<std::string, LoadedScriptInstance> instances{};
};

LuaJitScriptHost::LuaJitScriptHost()
    : vm_(std::make_unique<LuaSystem>()), impl_(std::make_unique<Impl>()) {}

LuaJitScriptHost::~LuaJitScriptHost() { Shutdown(); }

bool LuaJitScriptHost::Initialize() {
  if (ready_) {
    return true;
  }
  if (!vm_->Initialize()) {
    return false;
  }
  vm_->RegisterBinds();

  sol::state &state = vm_->GetState();
  sol::table packageTable = state["package"];
  if (packageTable.valid()) {
    const std::string path = packageTable["path"];
    packageTable["path"] =
        path + ";./Assets/Scripts/?.lua;./Assets/Scripts/?/init.lua";
  }

  ready_ = true;
  return true;
}

void LuaJitScriptHost::Shutdown() {
  if (impl_) {
    impl_->instances.clear();
  }
  ready_ = false;
}

std::string LuaJitScriptHost::NormalizeScriptRelativePath(
    const std::string_view scriptPath) {
  std::string normalized(scriptPath);
  if (normalized.empty()) {
    return normalized;
  }
  std::replace(normalized.begin(), normalized.end(), '\\', '/');
  const std::string prefix = "Assets/Scripts/";
  if (normalized.rfind(prefix, 0) == 0) {
    normalized.erase(0, prefix.size());
  }
  return normalized;
}

std::string LuaJitScriptHost::ResolveScriptFilePath(
    const std::string &relativePath) {
  namespace fs = std::filesystem;
  if (relativePath.empty()) {
    return {};
  }

  const std::array<std::string, 4> candidates = {
      "./Assets/Scripts/" + relativePath,
      "Assets/Scripts/" + relativePath,
      relativePath,
      "../Project/Assets/Scripts/" + relativePath,
  };

  for (const std::string &candidate : candidates) {
    if (fs::exists(candidate)) {
      return fs::weakly_canonical(fs::path(candidate)).string();
    }
  }
  return candidates.front();
}

bool LuaJitScriptHost::Invoke(const std::string_view scriptPath,
                              const LuaScriptHook hook,
                              const LuaScriptInvokeContext &context) {
  if (!ready_ || scriptPath.empty() || context.owner == nullptr) {
    return false;
  }

  if (!impl_) {
    impl_ = std::make_unique<Impl>();
  }

  const std::string relativePath = NormalizeScriptRelativePath(scriptPath);
  if (relativePath.empty()) {
    return false;
  }

  const std::string instanceKey =
      MakeInstanceKey(context.entityKey, relativePath);

  LoadedScriptInstance &instance = impl_->instances[instanceKey];
  if (!instance.valid) {
    const std::string absolutePath = ResolveScriptFilePath(relativePath);
    if (absolutePath.empty()) {
      return false;
    }

    sol::protected_function_result loadResult =
        vm_->GetState().script_file(absolutePath, sol::script_pass_on_error);
    if (!loadResult.valid()) {
      sol::error err = loadResult;
      std::cerr << "[LuaJitScriptHost] load failed: " << relativePath << "\n"
                << err.what() << "\n";
      D::LogFile::Instance()->Log(std::string("[LuaJitScriptHost] load failed: ") +
                                  relativePath + " " + err.what());
      return false;
    }

    sol::object returned = loadResult;
    if (!returned.is<sol::table>()) {
      std::cerr << "[LuaJitScriptHost] module must return a table: "
                << relativePath << "\n";
      return false;
    }

    instance.module = returned.as<sol::table>();
    instance.valid = true;
  }

  const char *functionName = HookFunctionName(hook);
  sol::object maybeFn = instance.module[functionName];
  if (!maybeFn.valid() || maybeFn.get_type() != sol::type::function) {
    return hook == LuaScriptHook::Resolve;
  }

  sol::protected_function fn = maybeFn;
  sol::protected_function_result callResult;
  if (hook == LuaScriptHook::Update) {
    callResult = fn(context.deltaTime);
  } else {
    callResult = fn();
  }

  if (!callResult.valid()) {
    sol::error err = callResult;
    std::cerr << "[LuaJitScriptHost] " << functionName << " failed: "
              << relativePath << "\n"
              << err.what() << "\n";
    D::LogFile::Instance()->Log(
        std::string("[LuaJitScriptHost] ") + functionName + " failed: " +
        relativePath + " " + err.what());
    return false;
  }

  return true;
}

void LuaJitScriptHost::ReleaseInstance(const std::string_view scriptPath,
                                       const uint64_t entityKey) {
  if (!impl_) {
    return;
  }
  const std::string relativePath = NormalizeScriptRelativePath(scriptPath);
  if (relativePath.empty()) {
    return;
  }
  impl_->instances.erase(MakeInstanceKey(entityKey, relativePath));
}

} // namespace Atrum::Script