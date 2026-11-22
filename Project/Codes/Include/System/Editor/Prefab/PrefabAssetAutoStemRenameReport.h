#pragma once

#include <string>

namespace Atrum::Editor::Prefab::PrefabAssetAutoStemRenameReport {

/// 無衝突の stem.json から stem_N へ振り分けたか
bool UsedAutoStemIncrementRename(const std::string &baseName,
                                 const std::string &outputPath);

} // namespace Atrum::Editor::Prefab::PrefabAssetAutoStemRenameReport