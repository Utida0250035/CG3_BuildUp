#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <memory>
#include <string>

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

/// ImGui・JSON 操作の UI 状態（シーンはプレビュー生成のみ担当）
struct UiState {
  std::string jsonFilePath = "../EditorAssets/Entities/editorTest.json";
  int selectedComponentIndex = 0;
  bool requestApplyPreview = false;
  bool requestReloadFromDisk = false;
  bool requestSaveToDisk = false;
};

/// エンティティ定義の編集・シリアライズ・プレビュー用 Entity 生成
class EntityEditor {
public:
  EntityEditor() = default;

  UiState &Ui() { return ui_; }
  const UiState &Ui() const { return ui_; }

  EntityDefinition &Document() { return document_; }
  const EntityDefinition &Document() const { return document_; }

  void LoadFromJsonPath(const std::string &jsonFilePath);
  void SaveToJsonPath(const std::string &jsonFilePath) const;

  /// Reload / Save / Apply フラグを処理。プレビュー再構築が必要なら true
  bool TickPendingRequests();

  /// 現在の Document からプレビュー用 Entity を生成（呼び出し側が所有）
  std::unique_ptr<Entity> BuildPreviewEntity() const;

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  void DrawEditorUi();
#endif

private:
  UiState ui_{};
  EntityDefinition document_{};
};

} // namespace Atrum::EntityEditor
