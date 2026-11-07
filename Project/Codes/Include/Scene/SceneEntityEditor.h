#pragma once

#include "Scene.h"
#include <memory>

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
  void ExitScene() override;

  static SceneEntityEditor *Instance();

  bool IsStageViewCameraLocked() const override { return false; }

protected:
  void DrawScene() override;

private:
  std::unique_ptr<EntityEditor::EntityEditor> editor_{};
  std::unique_ptr<Entity> previewEntity_{};

  void SetupPreviewCamera();
  void RebuildPreviewEntity();
};

} // namespace Atrum
