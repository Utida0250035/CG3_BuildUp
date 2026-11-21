#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

/// シーン内エディット（静止）とプレイ（シミュレーション）
enum class EditorInteractionMode { Edit, Play };

/// Imported へコピーするときの同名衝突方針
enum class ImportedAssetNameCollisionPolicy {
  AutoIncrementStem,
  OverwriteExisting,
  TimestampSuffix,
  BatchPrefixThenIncrement,
};

/// Entities へプレハブ JSON を新規出力するときの同名衝突方針
enum class PrefabAssetNameCollisionPolicy {
  AutoIncrementStem,
  OverwriteExisting,
  TimestampSuffix,
  BatchPrefixThenIncrement,
};

/// ImGui・JSON 操作の UI 状態（シーンはプレビュー生成のみ担当）
struct UiState {
  /// 編集中シーン JSON（既定は Scenes フォルダ）
  std::string jsonFilePath = "../EditorAssets/Scenes/Scene_0.json";
  /// プロジェクトパネルで選択中のシーンファイル
  std::string selectedScenePath{};
  int selectedInstanceIndex = 0;
  /// ヒエラルキー複数選択（空のときは selectedInstanceIndex のみ）
  std::vector<int> selectedInstanceIndices{};
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
  /// 保存後にエディタ内 Runtime ビューでシミュレーションする
  bool requestPlayRuntimeScene = false;
  /// Game/Runtime ビューで BuildUp 相当の描画を行う（プレビュー Entity のみ描画を避ける）
  bool runtimeFunctionalViewDraw = true;
  /// SceneBuildUp へ遷移して JSON シーンをフルランタイム実行する
  bool requestLaunchSceneBuildUp = false;
  /// 空なら jsonFilePath を SceneBuildUp に渡す
  std::string sceneBuildUpLaunchPath{};
  /// パーティクルエディタシーンへ遷移する
  bool requestOpenParticleEditor = false;
  /// シーン削除確認ポップアップを開く
  bool requestDeleteSceneConfirm = false;
  std::string pendingDeleteScenePath{};
  /// 未保存のまま別シーンを開く確認
  bool requestSceneSwitchUnsavedPrompt = false;
  std::string pendingSceneSwitchPath{};
  /// 実行時ビューウィンドウへフォーカス（シーン実行後など）
  bool requestFocusRuntimeSceneView = false;
  /// 次フレームで EntityEditor の Dock 既定レイアウトを再構築する
  bool requestResetDefaultDockLayout = false;
  /// 直近に Imported へコピーしたアセット（obj / png 等）
  std::string lastImportedAssetPath{};
  /// 直近 1 回のドロップでコピー成功したパス（古い順）
  std::vector<std::string> recentImportedAssetPaths{};
  std::string importStatusMessage{};
  ImportedAssetNameCollisionPolicy importCollisionPolicy =
      ImportedAssetNameCollisionPolicy::AutoIncrementStem;
  /// `BatchPrefixThenIncrement` 用（空ならファイル名のみ連番）
  std::string importBatchNamePrefix{};

  std::string tagSearchFilter{};
  std::string selectedCatalogTag{};
  std::string newCatalogTagName{};
  std::string renameCatalogTagName{};
  std::string newEntityTagName{};
  /// コンポーネント設定のコピー／ペースト用
  std::optional<ComponentSlot> clipboardComponentSlot{};
  /// プレハブ「シーンへ追加」／一括配置の件数（1–256）
  int prefabSpawnBatchCount = 1;
  /// プレハブ複数配置時の X 間隔
  float prefabSpawnSpacingX = 1.25f;
  PrefabAssetNameCollisionPolicy prefabOutputCollisionPolicy =
      PrefabAssetNameCollisionPolicy::AutoIncrementStem;
  /// `BatchPrefixThenIncrement` 用（プレハブ新規出力）
  std::string prefabOutputBatchNamePrefix{};
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
  bool DuplicateSelectedInstance();
  bool AddInstanceFromPrefabPath(const std::string &prefabPath);

  /// 選択中インスタンスの定義を EntityDefinition JSON として出力
  bool SaveSelectedInstanceToPrefabPath(const std::string &prefabPath);

  void LoadFromJsonPath(const std::string &jsonFilePath);
  void SaveToJsonPath(const std::string &jsonFilePath);

  bool IsSceneDocumentDirty() const { return sceneDocumentDirty_; }
  void MarkSceneDocumentDirty() { sceneDocumentDirty_ = true; }

  /// 新規シーン JSON を Scenes フォルダに作成して開く
  void CreateNewSceneFile();
  /// 別シーン JSON を編集対象として開く（未保存時は確認を挟む）
  bool TryOpenSceneFromPath(const std::string &sceneJsonPath);
  /// 未保存確認なしで開く（保存後・モーダル確定後）
  bool OpenSceneFromPath(const std::string &sceneJsonPath);
  /// ディスク上のシーン JSON を削除（編集中なら別シーンへ切替）
  bool DeleteSceneFile(const std::string &sceneJsonPath);

  /// Reload / Save / Apply フラグを処理。プレビュー再構築が必要なら true
  bool TickPendingRequests();

  /// SDL ドロップキューを処理し EditorAssets/Imported へコピー
  void ProcessDroppedFileImports();

  /// 現在のシーン定義からプレビュー用 Entity 列を生成（親子付け済み）
  std::vector<std::unique_ptr<Entity>> BuildPreviewScene();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  /// ドック分割レイアウト（シーンビュー矩形を EditorFrame に登録）
  void LayoutEditorWorkbench();
  void DrawHierarchyPanel();
  void DrawProjectPanel();
  void DrawInspectorPanel();
  void DrawTagsPanel();
#endif

  void ClampSelectedInstanceIndex();

  /// 正常読込後に Document を差し替え（タグ同期・選択リセット含む）
  void ApplySceneDocumentLoadedFromDisk(EditorSceneDocument document);
  /// パス不存在時の空シーン開始（通知は呼び出し側の status）
  void ApplyEmptySceneForMissingJsonPath(const std::string &jsonFilePath);

private:
  void ClearSceneDocumentDirty() { sceneDocumentDirty_ = false; }
  void ResetSelectionAfterSceneDocumentReload();

  UiState ui_{};
  EditorSceneDocument scene_{};
  bool sceneDocumentDirty_ = false;
};

} // namespace Atrum::EntityEditor
