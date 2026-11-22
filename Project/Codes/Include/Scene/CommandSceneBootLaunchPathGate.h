#pragma once

#include <string>

namespace Atrum {

/// Launch（BuildUp 生成）前のパスゲート。JSON 本体の読込は SceneBuildUp::EnterScene に任せる。
bool CanLaunchBootSceneAtPath(const std::string &sceneJsonPath);

} // namespace Atrum