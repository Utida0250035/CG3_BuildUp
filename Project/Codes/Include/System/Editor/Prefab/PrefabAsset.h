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

void EnsureEntitiesPrefabDirectoryExists();

/// 同名があれば番号付きで Entities 配下のパスを返す
std::string MakeUniquePrefabFilePath(const std::string &baseName);

} // namespace Atrum::Editor::Prefab
