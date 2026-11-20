#include "Scene/SceneBuildUpRegistryLaunchDedupe.h"

#include "Scene/SceneBuildUp.h"

#include <algorithm>
#include <memory>

namespace Atrum::SceneBuildUpRegistryLaunchDedupe {

namespace {

bool MatchesResolvedPath(const SceneBuildUp &scene,
                         const std::string &resolvedJsonPath) {
  return scene.SceneJsonPath() == resolvedJsonPath;
}

} // namespace

SceneBuildUp *AcquireOrCreateOwned(
    std::vector<std::unique_ptr<SceneBuildUp>> &ownedScenes,
    const std::string &resolvedJsonPath, Scene *returnScene) {
  SceneBuildUp *keeper = nullptr;
  for (const std::unique_ptr<SceneBuildUp> &owned : ownedScenes) {
    if (!owned || !MatchesResolvedPath(*owned, resolvedJsonPath)) {
      continue;
    }
    if (keeper == nullptr) {
      keeper = owned.get();
    }
  }

  if (keeper != nullptr) {
    keeper->SetReturnScene(returnScene);
    ownedScenes.erase(
        std::remove_if(
            ownedScenes.begin(), ownedScenes.end(),
            [&](const std::unique_ptr<SceneBuildUp> &owned) {
              return owned && MatchesResolvedPath(*owned, resolvedJsonPath) &&
                     owned.get() != keeper;
            }),
        ownedScenes.end());
    return keeper;
  }

  auto scene = std::make_unique<SceneBuildUp>(resolvedJsonPath, returnScene);
  SceneBuildUp *const raw = scene.get();
  ownedScenes.push_back(std::move(scene));
  return raw;
}

} // namespace Atrum::SceneBuildUpRegistryLaunchDedupe