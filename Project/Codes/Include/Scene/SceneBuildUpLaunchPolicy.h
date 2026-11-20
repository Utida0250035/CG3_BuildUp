#pragma once

namespace Atrum {

class Scene;

/// SceneBuildUpRegistry::Launch の呼び手整理（生成＋フェード遷移を 1 API）。
namespace SceneBuildUpLaunchPolicy {

void QueueFadeToBuildUp(const std::string &sceneJsonPath, Scene *returnScene);

} // namespace SceneBuildUpLaunchPolicy

} // namespace Atrum