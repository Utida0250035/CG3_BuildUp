#pragma once

#include <string>

namespace Atrum {

/// atomF シーンなら parameters.json を読み、それ以外ではランタイム保持を解放する。
void SyncAtomFParametersWithScenePath(const std::string &sceneJsonPath);

} // namespace Atrum