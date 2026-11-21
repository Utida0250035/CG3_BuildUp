#pragma once

#include "System/Scripting/ILuaBinder.h"
#include <sol/forward.hpp>
#include <memory>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum::Script {

struct LuaEntityApiContext {
  std::vector<std::unique_ptr<Entity>> *entities = nullptr;
  std::vector<sol::protected_function> *tickCallbacks = nullptr;
};

/// Atrum.* 名前空間へエンティティ生成 API などを登録する
class LuaEntityApiBinder final : public ILuaBinder {
public:
  explicit LuaEntityApiBinder(LuaEntityApiContext context);

  void Register(sol::state &lua) override;

private:
  LuaEntityApiContext context_{};
};

} // namespace Atrum::Script