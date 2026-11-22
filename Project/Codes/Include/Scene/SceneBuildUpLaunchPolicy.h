#pragma once

#include <string>

namespace Atrum {

class Scene;

/// SceneBuildUp の生成と遷移キュー投入を呼び手向けに整理（Registry::Launch は生成のみ）。
namespace SceneBuildUpLaunchPolicy {

/// SceneBuildUp を生成して所有登録する。EnterScene は呼び出し側／CommandChangeScene が行う。
Scene *CreateOwnedBuildUp(const std::string &sceneJsonPath, Scene *returnScene);

void QueueFadeToBuildUp(const std::string &sceneJsonPath, Scene *returnScene);

} // namespace SceneBuildUpLaunchPolicy

} // namespace Atrum