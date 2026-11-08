#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

/// シーン内エディット（静止）とプレイ（シミュレーション）
enum class EditorInteractionMode { Edit, Play };

/// ImGui・JSON 操作の UI 状態（シーンはプレビュー生成のみ担当）
struct UiState {
  /// 編集中シーン JSON（既定は Scenes フォルダ）
  std::string jsonFilePath = "../EditorAssets/Scenes/Scene_0.json";
  /// プロジェクトパネルで選択中のシーンファイル
  std::string selectedScenePath{};
  int selectedInstanceIndex = 0;
  std::string selectedPrefabPath{};
  ComponentSlotKind selectedComponentKind = ComponentSlotKind::Draw;
  int selectedComponentIndex = 0;
  bool requestApplyPreview = false;
  bool requestReloadFromDisk = false;
  bool requestSaveToDisk = false;
  EditorInteractionMode interactionMode = EditorInteractionMode::Edit;
  /// true のときだけプレビュー Entity に Update を回す
  bool previewSimulationEnabled = false;
  /// 現在の Document からプレビューを作り直す（初期状態へ戻す）
  bool requestResetPreview = false;
  /// 保存後に SceneBuildUp へ遷移してシーンを実行する
  bool requestPlayRuntimeScene = false;
  /// パーティクルエディタシーンへ遷移する
  bool requestOpenParticleEditor = false;
  /// シーン削除確認ポップアップを開く
  bool requestDeleteSceneConfirm = false;
  std::string pendingDeleteScenePath{};
  /// 実行時ビューウィンドウへフォーカス（シーン実行後など）
  bool requestFocusRuntimeSceneView = false;
  /// 直近に Imported へコピーしたアセット（obj / png 等）
  std::string lastImportedAssetPath{};
  std::string importStatusMessage{};

  std::string tagSearchFilter{};
  std::string selectedCatalogTag{};
  std::string newCatalogTagName{};
  std::string renameCatalogTagName{};
  std::string newEntityTagName{};
};

/// エンティティ定義の編集・シリアライズ・プレビュー用 Entity 生成
class EntityEditor {
public:
  EntityEditor() = default;

  UiState &Ui() { return ui_; }
  const UiState &Ui() const { return ui_; }

  EditorSceneDocument &SceneDocument() { return scene_; }
  const EditorSceneDocument &SceneDocument() const { return scene_; }

  EntityDefinition &SelectedDefinition();
  const EntityDefinition &SelectedDefinition() const;
  SceneEntityInstance &SelectedInstance();
  const SceneEntityInstance &SelectedInstance() const;

  void AddEmptyInstance();
  void RemoveSelectedInstance();
  bool AddInstanceFromPrefabPath(const std::string &prefabPath);

  /// 選択中インスタンスの定義を EntityDefinition JSON として出力
  bool SaveSelectedInstanceToPrefabPath(const std::string &prefabPath);

  void LoadFromJsonPath(const std::string &jsonFilePath);
  void SaveToJsonPath(const std::string &jsonFilePath);

  /// 新規シーン JSON を Scenes フォルダに作成して開く
  void CreateNewSceneFile();
  /// 別シーン JSON を編集対象として開く
  bool OpenSceneFromPath(const std::string &sceneJsonPath);
  /// ディスク上のシーン JSON を削除（編集中なら別シーンへ切替）
  bool DeleteSceneFile(const std::string &sceneJsonPath);

  /// Reload / Save / Apply フラグを処理。プレビュー再構築が必要なら true
  bool TickPendingRequests();

  /// SDL ドロップキューを処理し EditorAssets/Imported へコピー
  void ProcessDroppedFileImports();

  /// 現在のシーン定義からプレビュー用 Entity 列を生成（親子付け済み）
  std::vector<std::unique_ptr<Entity>> BuildPreviewScene() const;

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  /// ドック分割レイアウト（シーンビュー矩形を EditorFrame に登録）
  void LayoutEditorWorkbench();
  void DrawHierarchyPanel();
  void DrawProjectPanel();
  void DrawInspectorPanel();
  void DrawTagsPanel();
#endif

  void ClampSelectedInstanceIndex();

private:
  UiState ui_{};
  EditorSceneDocument scene_{};
};

} // namespace Atrum::EntityEditor
