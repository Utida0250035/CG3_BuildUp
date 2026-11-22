#include "Scene/SceneBuildUpAtomFParametersBoot.h"

#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/GameAssetsPaths.h"

namespace Atrum {

void SyncAtomFParametersWithScenePath(const std::string &sceneJsonPath) {
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    AtomF::ParametersRuntime::Instance().Unload();
    return;
  }

  AtomF::ParametersRuntime::Instance().TryLoadFromJsonFile(
      GameAssets::kAtomFParametersJson);
}

} // namespace Atrum