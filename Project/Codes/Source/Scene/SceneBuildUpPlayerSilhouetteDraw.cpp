#include "Scene/SceneBuildUpPlayerSilhouetteDraw.h"

#include "System/Entity/Components/CmpParry.h"
#include "System/Entity/Entity.h"
#include "System/PlayerParryReach.h"
#include "Visual/PlayerTerrainSilhouette.h"

namespace Atrum::SceneBuildUpPlayerSilhouette {

namespace {

const Entity *FindPlayerEntityForSilhouette(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  for (const std::unique_ptr<Entity> &entity : entities) {
    if (!entity || !entity->IsActive()) {
      continue;
    }
    if (entity->GetParent() != nullptr) {
      continue;
    }
    if (entity->HasDataCmp<CmpParry>()) {
      return entity.get();
    }
  }
  return nullptr;
}

} // namespace

void DrawAfterRuntimeEntities(
    AtrumEngine *engine,
    const std::vector<std::unique_ptr<Entity>> &entities) {
  if (engine == nullptr) {
    return;
  }

  const Entity *player = FindPlayerEntityForSilhouette(entities);
  if (player == nullptr) {
    return;
  }

  PlayerTerrainSilhouette::DrawOccludedSilhouette(
      engine, player->GetWorldMatrix(), kParryColliderRadius);
}

} // namespace Atrum::SceneBuildUpPlayerSilhouette