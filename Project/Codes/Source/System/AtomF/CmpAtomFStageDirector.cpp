#include "System/AtomF/CmpAtomFStageDirector.h"

#include "System/AtomF/AtomFEnemySpawn.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Scripting/ScriptSceneContext.h"

#include <memory>
#include <vector>

namespace Atrum {

void CmpAtomFStageDirector::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFStageDirector::Update() {
  if (frameDeltaTime_ == nullptr || bossCalled_) {
    return;
  }
  const std::vector<std::unique_ptr<Entity>> *entities =
      Script::SceneContext::Entities();
  if (entities == nullptr) {
    return;
  }

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  int minionCount = 0;
  CmpAtomFBoss *boss = nullptr;
  for (const std::unique_ptr<Entity> &e : *entities) {
    if (e == nullptr || !e->IsActive()) {
      continue;
    }
    if (CmpAtomFBoss *b = e->GetUpdCmp<CmpAtomFBoss>()) {
      boss = b;
      continue;
    }
    if (tags.EntityHasRole(*e, "enemy")) {
      ++minionCount;
    }
  }

  if (minionCount > 0) {
    clearedElapsedSeconds_ = 0.0f;
    return;
  }

  const AtomF::StageParameters &stage = AtomF::ResolveStageParameters();
  clearedElapsedSeconds_ += frameDeltaTime_->GetDeltaTime();

  if (nextWaveIndex_ < stage.waveEnemyCounts.size()) {
    if (clearedElapsedSeconds_ < stage.waveStartDelaySeconds) {
      return;
    }
    const int count = stage.waveEnemyCounts[nextWaveIndex_];
    // 群れごとに配置の角度をずらす。
    AtomF::SpawnAtomFEnemyWave(count, stage.waveSpawnRadiusMeters,
                               0.45f * static_cast<float>(nextWaveIndex_ + 1));
    ++nextWaveIndex_;
    clearedElapsedSeconds_ = -0.5f; // 採用されるまでの猶予
    return;
  }

  if (clearedElapsedSeconds_ < stage.bossAppearDelaySeconds) {
    return;
  }
  if (boss != nullptr) {
    boss->BeginAppear();
  }
  bossCalled_ = true;
}

} // namespace Atrum
