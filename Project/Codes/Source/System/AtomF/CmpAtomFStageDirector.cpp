#include "System/AtomF/CmpAtomFStageDirector.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFBossSpawn.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Scripting/ScriptSceneContext.h"

namespace Atrum {

namespace {

int CountActiveEnemies() {
  const std::vector<std::unique_ptr<Entity>> *entities =
      Script::SceneContext::Entities();
  if (entities == nullptr) {
    return 0;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  int count = 0;
  for (const std::unique_ptr<Entity> &entity : *entities) {
    if (entity != nullptr && entity->IsActive() &&
        tags.EntityHasRole(*entity, "enemy")) {
      ++count;
    }
  }
  return count;
}

} // namespace

void CmpAtomFStageDirector::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFStageDirector::Update() {
  if (frameDeltaTime_ == nullptr || bossSpawned_) {
    return;
  }

  const int enemyCount = CountActiveEnemies();
  if (enemyCount > 0) {
    seenEnemy_ = true;
    clearedElapsedSeconds_ = 0.0f;
    return;
  }
  if (!seenEnemy_) {
    return;
  }

  clearedElapsedSeconds_ += frameDeltaTime_->GetDeltaTime();
  const AtomF::BossParameters &boss = AtomF::ResolveBossParameters();
  if (clearedElapsedSeconds_ < boss.appearDelayAfterMinionsSeconds) {
    return;
  }

  bossSpawned_ = true;
  AtomF::EnqueueSceneEntityAdopt(
      AtomF::CreateBossEntity(Math::Vector3{0.0f, 0.0f, boss.spawnZMeters}));
}

} // namespace Atrum
