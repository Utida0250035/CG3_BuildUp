#pragma once

#include <string>

namespace Atrum::Editor::Prefab {

/// Entities 配下のプレハブ JSON を削除（ルート外は拒否）
bool TryDeletePrefabJsonFile(const std::string &prefabPath);

/// Entities 配下でプレハブ JSON を改名。成功時は新パス、失敗時は空文字
std::string TryRenamePrefabJsonFile(const std::string &prefabPath,
                                    const std::string &newFileName);

} // namespace Atrum::Editor::Prefab