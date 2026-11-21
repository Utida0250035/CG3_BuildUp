#pragma once

#include "Scene.h"
#include <memory>
#include <vector>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/EditorFrame.h"
#endif

namespace Atrum {

class Entity;

namespace EntityEditor {
class EntityEditor;
} // namespace EntityEditor

/// エンティティ定義の編集・プレビュー専用シーン（Development + ImGui）
class SceneEntityEditor : public Scene {
public:
  SceneEntityEditor();
  ~SceneEntityEditor() override;

  void EnterScene() override;
  void Update() override;
  void Draw() override;
  void ExitScene() override;

  static SceneEntityEditor *Instance();

  /// BuildUp から戻った直後にエディタのシーンビューを Edit へ戻す
  static void RestoreUiAfterSceneBuildUpExit();

  /// BuildUp 入場時: Runtime タブ固定はせず、残ったフォーカス要求だけ解除
  static void ClearStaleRuntimeSceneViewFocusRequest();

  /// CommandScene 起動時に編集対象 JSON を先に指定する
  void SetBootJsonPath(const std::string &jsonFilePath);

  bool IsStageViewCameraLocked() const override { return false; }

  /// GameOrder では更新しない。エディットビュー上は Draw 内で DebugCamera を操作する。
  bool AllowDebugCameraControl() const override;

  const std::vector<std::unique_ptr<Entity>> *
  GetSupplementalCameraEntities() const override;

  /// Game タブ描画前に戦闘ステージ相当のカメラを適用する。
  void ApplyGameViewStageCameraForDraw();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  EntityEditor::EntityEditor *MutableEntityEditor() { return editor_.get(); }
#endif

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  /// Edit ビューホバー時のみ CmpCamera へ Debug 入力を適用する。
  void ApplyEditViewHoveredDebugCameraBeforeDraw();
#endif

protected:
  void DrawScene() override;

private:
  std::unique_ptr<EntityEditor::EntityEditor> editor_{};
  std::vector<std::unique_ptr<Entity>> previewEntities_{};
  std::string pendingBootJsonPath_{};

  void SetupPreviewCamera();
  void RebuildPreviewScene();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  void DrawSceneViewContent(Atrum::Editor::SceneViewTab viewTab, int width,
                            int height);
#endif
  void DrawPreviewEntities();
};

} // namespace Atrum
