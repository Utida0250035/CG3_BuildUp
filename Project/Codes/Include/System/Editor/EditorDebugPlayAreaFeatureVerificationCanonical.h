#pragma once

#include "System/Editor/EditorAssetsPaths.h"

namespace Atrum::Editor::DebugPlayAreaFeatureVerificationCanonical {

/// リポジトリ同梱の機能検証シーン JSON（FeatureTest）の正本パス。
inline const char *SceneJsonPath() {
  return EditorAssets::kDebugPlayAreaFeatureTestScene;
}

} // namespace Atrum::Editor::DebugPlayAreaFeatureVerificationCanonical