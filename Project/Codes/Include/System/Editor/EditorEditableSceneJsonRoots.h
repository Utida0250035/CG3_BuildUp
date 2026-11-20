#pragma once

#include "System/Editor/EditorAssetsPaths.h"
#include "System/GameAssetsPaths.h"
#include <array>
#include <cstddef>

namespace Atrum::Editor::EditorEditableSceneJsonRoots {

/// エディタ「編集可能シーン」一覧の走査ルート（単一 Dir ではない）
inline constexpr std::array<const char *, 3> kAll = {
    GameAssets::kScenesDirectory,
    EditorAssets::kScenesDirectory,
    EditorAssets::kDebugPlayAreaDirectory,
};

inline constexpr std::size_t kCount = kAll.size();

} // namespace Atrum::Editor::EditorEditableSceneJsonRoots