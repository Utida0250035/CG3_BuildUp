#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrame.h"
#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::Editor {

/// エディタ上のシーン実行フェーズ（UI 表示用）
enum class EditorSceneRunPhase {
  Waiting,
  Paused,
  Running,
};

const char *EditorSceneRunPhaseLabel(EditorSceneRunPhase phase);

/// エンティティエディタの UI 状態から表示フェーズを解決する
EditorSceneRunPhase QueryEntityEditorSceneRunPhase(
    const EntityEditor::UiState &ui, SceneViewTab viewTab,
    bool sceneTransitionActive, bool playRequestPending);

void DrawEditorSceneRunPhaseBadge(EditorSceneRunPhase phase);

void SetEntityEditorSceneMode(EntityEditor::UiState &ui, const bool wantRun);

/// メニューバー／ヒエラルキー等に置くシーン↔実行トグルと状態バッジ
void DrawEntityEditorSceneModeToolbar(EntityEditor::EntityEditor &editor,
                                      const EditorSceneRunPhase runPhase);

} // namespace Atrum::Editor

#endif
