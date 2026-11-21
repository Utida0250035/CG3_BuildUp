#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum::Script {

struct LuaComponentAttachResult {
  bool success = false;
  std::string message{};
};

Entity *FindEntityByNameHash(std::vector<std::unique_ptr<Entity>> &entities,
                             const std::string &entityName);

LuaComponentAttachResult TryAttachLuaUpdByEntityName(
    std::vector<std::unique_ptr<Entity>> &entities,
    const std::string &entityName, const std::string &scriptPath,
    const std::string &updCmpGroup = "MOVEMENT");

LuaComponentAttachResult TryAttachLuaDrawByEntityName(
    std::vector<std::unique_ptr<Entity>> &entities,
    const std::string &entityName, const std::string &scriptPath,
    const std::string &drawCmpGroup = "NONE");

} // namespace Atrum::Script