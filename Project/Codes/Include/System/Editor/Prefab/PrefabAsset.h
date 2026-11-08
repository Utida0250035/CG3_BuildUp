#pragma once

#include "System/Editor/EditorAssetsPaths.h"
#include <string>
#include <vector>

namespace Atrum::Editor::Prefab {

constexpr const char *kEntitiesPrefabDirectory =
    EditorAssets::kEntitiesPrefabDirectory;

/// フォルダ内の *.json プレハブパス一覧（相対パス）
std::vector<std::string> ListPrefabJsonFiles(
    const std::string &directoryRelativePath = kEntitiesPrefabDirectory);

} // namespace Atrum::Editor::Prefab
