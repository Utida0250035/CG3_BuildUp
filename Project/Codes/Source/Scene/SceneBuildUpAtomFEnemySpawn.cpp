#include "Scene/SceneBuildUpAtomFEnemySpawn.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFBossSpawn.h"
#include "System/AtomF/AtomFEnemySpawn.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr int kAtomFEnemySpawnCount = 6;

} // namespace

void AppendAtomFEnemyIfScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    return;
  }

  // 円周上に複数配置し、連鎖爆発が起きやすい配置にする。
  constexpr float kSpawnRadiusMeters = 8.0f;
  constexpr float kTwoPi = 6.28318530718f;
  for (int i = 0; i < kAtomFEnemySpawnCount; ++i) {
    const float angle =
        (kTwoPi * static_cast<float>(i)) / static_cast<float>(kAtomFEnemySpawnCount);
    const Math::Vector3 position{std::cos(angle) * kSpawnRadiusMeters, 0.0f,
                                 std::sin(angle) * kSpawnRadiusMeters};
    entities.push_back(AtomF::CreateAtomFEnemyEntity(position, i));
  }

  // ボス（未出現）とステージ進行役。最後の群れが全滅するとボスが出現する。
  entities.push_back(AtomF::CreateAtomFBossEntity());
  entities.push_back(AtomF::CreateAtomFStageDirectorEntity());
}

} // namespace Atrum
