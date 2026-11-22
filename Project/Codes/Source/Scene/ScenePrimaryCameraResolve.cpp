#include "Scene/ScenePrimaryCameraResolve.h"

#include "System/Entity/Components/CmpCamera.h"
#include "System/Entity/Entity.h"
#include "System/Hash64.h"

namespace Atrum {

namespace {

constexpr const char *kMainCameraTag = "camera.main";

bool EntityHasMainCameraTag(const Entity &entity) {
  return entity.HasTagHash(Hash64(kMainCameraTag));
}

const Entity *FindPrimaryCameraEntity(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  const Entity *primaryEntity = nullptr;
  for (const auto &entity : entities) {
    if (!entity || !entity->HasUpdCmp<CmpCamera>()) {
      continue;
    }
    if (EntityHasMainCameraTag(*entity)) {
      return entity.get();
    }
    if (primaryEntity == nullptr) {
      primaryEntity = entity.get();
    }
  }
  return primaryEntity;
}

const CmpCamera *ResolveFromSupplemental(
    const std::vector<std::unique_ptr<Entity>> *supplementalEntities) {
  if (supplementalEntities == nullptr) {
    return nullptr;
  }
  const Entity *primaryEntity = FindPrimaryCameraEntity(*supplementalEntities);
  if (primaryEntity == nullptr) {
    return nullptr;
  }
  return primaryEntity->GetUpdCmp<CmpCamera>();
}

} // namespace

CmpCamera *ResolvePrimaryCmpCamera(
    const std::vector<std::unique_ptr<Entity>> &rigEntities,
    const std::vector<std::unique_ptr<Entity>> *supplementalEntities) {
  const Entity *primaryEntity = FindPrimaryCameraEntity(rigEntities);
  if (primaryEntity != nullptr) {
    return primaryEntity->GetUpdCmp<CmpCamera>();
  }
  return const_cast<CmpCamera *>(ResolveFromSupplemental(supplementalEntities));
}

const CmpCamera *ResolvePrimaryCmpCamera(
    const std::vector<std::unique_ptr<Entity>> &rigEntities,
    const std::vector<std::unique_ptr<Entity>> *supplementalEntities) {
  const Entity *primaryEntity = FindPrimaryCameraEntity(rigEntities);
  if (primaryEntity != nullptr) {
    return primaryEntity->GetUpdCmp<CmpCamera>();
  }
  return ResolveFromSupplemental(supplementalEntities);
}

} // namespace Atrum