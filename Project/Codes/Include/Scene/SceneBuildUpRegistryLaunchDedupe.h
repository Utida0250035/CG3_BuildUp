#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Scene;
class SceneBuildUp;

/// Launch 時の同一 resolved パス重複を抑止（再 Launch は既存インスタンスを返す）。
namespace SceneBuildUpRegistryLaunchDedupe {

SceneBuildUp *AcquireOrCreateOwned(
    std::vector<std::unique_ptr<SceneBuildUp>> &ownedScenes,
    const std::string &resolvedJsonPath, Scene *returnScene);

} // namespace SceneBuildUpRegistryLaunchDedupe

} // namespace Atrum