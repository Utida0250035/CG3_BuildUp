#pragma once

#include <string>

namespace Atrum::SceneBuildUpSceneDisplayNameResolve {

/// コンストラクタ用: JSON の sceneName を優先し、無ければパス stem
std::string ResolveInitialSceneDisplayName(const std::string &jsonFilePath);

} // namespace Atrum::SceneBuildUpSceneDisplayNameResolve