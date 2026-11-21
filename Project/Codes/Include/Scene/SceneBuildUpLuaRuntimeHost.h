#pragma once

#include <sol/forward.hpp>
#include <memory>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum::Script {
class LuaSystem;
}

namespace Atrum {

/// SceneBuildUp 用の Lua ランタイム（bootstrap と behavior onTick）
class SceneBuildUpLuaRuntimeHost final {
public:
  void OnEnterScene(std::vector<std::unique_ptr<Entity>> *entities);
  void OnUpdate();
  void OnExitScene();

  bool IsActive() const { return active_; }

private:
  std::unique_ptr<Script::LuaSystem> lua_{};
  std::vector<sol::protected_function> tickCallbacks_{};
  std::vector<std::unique_ptr<Entity>> *entities_ = nullptr;
  bool active_ = false;
};

} // namespace Atrum