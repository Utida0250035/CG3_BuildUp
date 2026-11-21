#include "System/Scripting/LuaEntityComponentAttach.h"

#include "System/Entity/Components/CmpLuaDraw.h"
#include "System/Entity/Components/CmpLuaUpd.h"
#include "System/Entity/Entity.h"
#include "System/Hash64.h"

namespace Atrum::Script {

Entity *FindEntityByNameHash(std::vector<std::unique_ptr<Entity>> &entities,
                             const std::string &entityName) {
  if (entityName.empty()) {
    return nullptr;
  }
  const uint64_t targetHash = Hash64(entityName);
  for (std::unique_ptr<Entity> &entity : entities) {
    if (entity != nullptr && entity->GetHash() == targetHash) {
      return entity.get();
    }
  }
  return nullptr;
}

namespace {

void ResolveIfActive(Entity &entity, ComponentUpd *component) {
  if (!entity.IsActive() || component == nullptr) {
    return;
  }
  component->ResolveDependence();
}

void ResolveIfActive(Entity &entity, ComponentDraw *component) {
  if (!entity.IsActive() || component == nullptr) {
    return;
  }
  component->ResolveDependence();
}

} // namespace

LuaComponentAttachResult TryAttachLuaUpdByEntityName(
    std::vector<std::unique_ptr<Entity>> &entities,
    const std::string &entityName, const std::string &scriptPath,
    const std::string &updCmpGroup) {
  LuaComponentAttachResult result{};
  Entity *entity = FindEntityByNameHash(entities, entityName);
  if (entity == nullptr) {
    result.message = "entity not found: " + entityName;
    return result;
  }

  if (!entity->HasUpdCmp<CmpLuaUpd>()) {
    entity->AddUpdCmp<CmpLuaUpd>();
  }
  CmpLuaUpd *cmp = entity->GetUpdCmp<CmpLuaUpd>();
  cmp->AddOrUpdateBinding(scriptPath, updCmpGroup);
  ResolveIfActive(*entity, cmp);

  result.success = true;
  result.message = "attached CmpLuaUpd: " + scriptPath;
  return result;
}

LuaComponentAttachResult TryAttachLuaDrawByEntityName(
    std::vector<std::unique_ptr<Entity>> &entities,
    const std::string &entityName, const std::string &scriptPath,
    const std::string &drawCmpGroup) {
  LuaComponentAttachResult result{};
  Entity *entity = FindEntityByNameHash(entities, entityName);
  if (entity == nullptr) {
    result.message = "entity not found: " + entityName;
    return result;
  }

  if (!entity->HasDrawCmp<CmpLuaDraw>()) {
    entity->AddDrawCmp<CmpLuaDraw>();
  }
  CmpLuaDraw *cmp = entity->GetDrawCmp<CmpLuaDraw>();
  cmp->AddOrUpdateBinding(scriptPath, drawCmpGroup);
  ResolveIfActive(*entity, cmp);

  result.success = true;
  result.message = "attached CmpLuaDraw: " + scriptPath;
  return result;
}

} // namespace Atrum::Script