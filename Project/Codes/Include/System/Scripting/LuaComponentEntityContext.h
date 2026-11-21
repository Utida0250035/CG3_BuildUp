#pragma once

#include <sol/forward.hpp>

namespace Atrum {
class Entity;
}

namespace Atrum::Script {

/// Lua コンポーネントの onResolve / onUpdate / onDraw に渡すコンテキスト
sol::table MakeLuaComponentEntityContext(sol::state &lua, Entity &entity);

} // namespace Atrum::Script