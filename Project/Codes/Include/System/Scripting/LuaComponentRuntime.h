#pragma once

namespace Atrum::Script {
class LuaSystem;
}

namespace sol {
class state;
}

namespace Atrum::Script {

/// SceneBuildUp の Lua ホストが有効な間だけ非 null（コンポーネントから参照）
class LuaComponentRuntime final {
public:
  static void SetActive(LuaSystem *luaSystem);
  static LuaSystem *ActiveSystem();
  static sol::state *ActiveState();

private:
  LuaComponentRuntime() = delete;
};

} // namespace Atrum::Script