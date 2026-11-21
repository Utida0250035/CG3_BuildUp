#include "System/Editor/EditorSceneRuntimeEntityInitializePolicy.h"

#include "System/Entity/Entity.h"

namespace Atrum::EntityEditor {

namespace {

bool NeedsSceneEntityInitialize(const Entity *entity) {
  return entity != nullptr && entity->GetState() != Entity::State::Active;
}

} // namespace

void InitializeSceneDocumentEntities(
    std::vector<std::unique_ptr<Entity>> &entities) {
  // シーン JSON の親子は BuildEntities 側で Entity::SetParent に反映済み。
  // ルートの Initialize が Entity::childs_ を再帰的に初期化し、取り残しは下の多段パスで補う。
  for (std::unique_ptr<Entity> &entity : entities) {
    if (entity) {
      entity->ResolveDependencies();
    }
  }

  for (std::unique_ptr<Entity> &entity : entities) {
    if (entity && entity->GetParent() == nullptr &&
        NeedsSceneEntityInitialize(entity.get())) {
      entity->Initialize();
    }
  }

  constexpr int kMaxOrphanInitPasses = 8;
  for (int pass = 0; pass < kMaxOrphanInitPasses; ++pass) {
    bool initializedAny = false;
    for (std::unique_ptr<Entity> &entity : entities) {
      if (NeedsSceneEntityInitialize(entity.get())) {
        entity->Initialize();
        initializedAny = true;
      }
    }
    if (!initializedAny) {
      break;
    }
  }
}

} // namespace Atrum::EntityEditor