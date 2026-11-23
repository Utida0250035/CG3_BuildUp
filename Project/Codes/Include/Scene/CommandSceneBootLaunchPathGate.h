#pragma once

#include <string>

namespace Atrum {

/// JSON ファイルの実在と EditorSceneDocument としての読込可否を検証する。
bool IsValidBootSceneJsonPath(const std::string &sceneJsonPath);

/// Launch（BuildUp 生成）前のパスゲート。JSON 本体の読込は SceneBuildUp::EnterScene に任せる。
bool CanLaunchBootSceneAtPath(const std::string &sceneJsonPath);

} // namespace Atrum