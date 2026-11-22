#pragma once

#include <string>

namespace Atrum::EntityEditor {
enum class PrefabAssetNameCollisionPolicy;
}

namespace Atrum::Editor::Prefab {

/// 無衝突なら stem.json。衝突時は方針に従いファイル名を決定（Entities 配下の相対パス）
std::string ResolvePrefabJsonOutputPath(
    const std::string &baseName,
    EntityEditor::PrefabAssetNameCollisionPolicy policy,
    const std::string &batchNamePrefix = {});

} // namespace Atrum::Editor::Prefab