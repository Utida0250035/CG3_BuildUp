#include "Scene/SceneBuildUpAtomFEnemySpawn.h"

#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFStageDirector.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void AppendAtomFEnemyIfScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    return;
  }

  // 敵・ボスの出現は進行役（parameters.json の stage.waves / boss）が行う。
  std::unique_ptr<Entity> director =
      std::make_unique<Entity>("AtomFStageDirector");
  director->AddUpdCmp<CmpAtomFStageDirector>();
  director->ResolveDependencies();
  director->Initialize();
  entities.push_back(std::move(director));
}

} // namespace Atrum
