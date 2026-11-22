#pragma once

namespace Atrum::Editor::Scene {

/// PlayRequest が保持する起動意図（パス以外のメタデータ）
enum class EditorScenePlayStagingKind {
  Unset = 0,
  BuildUpLaunch,
  InEditorRuntimePreview,
  RepositoryStartup,
};

} // namespace Atrum::Editor::Scene