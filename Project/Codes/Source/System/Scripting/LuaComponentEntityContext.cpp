#include "System/Scripting/LuaComponentEntityContext.h"

#include "System/Entity/Entity.h"

namespace Atrum::Script {

sol::table MakeLuaComponentEntityContext(sol::state &lua, Entity &entity) {
  sol::table context = lua.create_table();
  context["entityId"] = entity.GetId();
  const Math::TransformLH &transform = entity.RefTransform();
  sol::table position = lua.create_table();
  position["x"] = transform.translate.x;
  position["y"] = transform.translate.y;
  position["z"] = transform.translate.z;
  context["position"] = position;
  context["active"] = entity.IsActive();
  return context;
}

} // namespace Atrum::Script