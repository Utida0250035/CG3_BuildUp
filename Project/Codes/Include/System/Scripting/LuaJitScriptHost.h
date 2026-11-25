#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace Atrum {

class Entity;
class ComponentUpd;

namespace Script {

class LuaSystem;

enum class LuaScriptHook {
  Resolve,
  Update,
};

struct LuaScriptInvokeContext {
  Entity *owner = nullptr;
  ComponentUpd *sourceComponent = nullptr;
  uint64_t entityKey = 0;
  float deltaTime = 0.0f;
};

class LuaJitScriptHost {
public:
  LuaJitScriptHost();
  ~LuaJitScriptHost();

  LuaJitScriptHost(const LuaJitScriptHost &) = delete;
  LuaJitScriptHost &operator=(const LuaJitScriptHost &) = delete;

  bool Initialize();
  void Shutdown();

  bool IsReady() const { return ready_; }

  bool Invoke(std::string_view scriptPath, LuaScriptHook hook,
              const LuaScriptInvokeContext &context);

  void ReleaseInstance(std::string_view scriptPath, uint64_t entityKey);

private:
  struct Impl;

  std::unique_ptr<LuaSystem> vm_;
  std::unique_ptr<Impl> impl_;
  bool ready_ = false;

  static std::string NormalizeScriptRelativePath(std::string_view scriptPath);
  static std::string ResolveScriptFilePath(const std::string &relativePath);
};

} // namespace Script
} // namespace Atrum