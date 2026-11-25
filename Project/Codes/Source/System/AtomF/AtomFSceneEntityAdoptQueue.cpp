#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"

#include "System/Entity/Entity.h"

#include <utility>
#include <vector>

namespace Atrum::AtomF {
namespace {

std::vector<std::unique_ptr<Entity>> &PendingAdoptEntities() {
  static std::vector<std::unique_ptr<Entity>> pending;
  return pending;
}

} // namespace

void EnqueueSceneEntityAdopt(std::unique_ptr<Entity> entity) {
  if (entity == nullptr) {
    return;
  }
  PendingAdoptEntities().push_back(std::move(entity));
}

void FlushSceneEntityAdoptQueue(
    std::vector<std::unique_ptr<Entity>> &entities) {
  std::vector<std::unique_ptr<Entity>> &pending = PendingAdoptEntities();
  if (pending.empty()) {
    return;
  }
  for (std::unique_ptr<Entity> &entity : pending) {
    if (entity != nullptr) {
      entities.push_back(std::move(entity));
    }
  }
  pending.clear();
}

void ClearSceneEntityAdoptQueue() {
  std::vector<std::unique_ptr<Entity>> &pending = PendingAdoptEntities();
  for (std::unique_ptr<Entity> &entity : pending) {
    if (entity != nullptr) {
      entity->Finalize();
    }
  }
  pending.clear();
}

} // namespace Atrum::AtomF
