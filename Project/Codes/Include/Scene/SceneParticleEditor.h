#pragma once

#include "Scene.h"
#include "System/Particle/ParticleParam.h"
#include <memory>
#include <string>
#include <vector>

namespace Atrum::Visual::Particle {
class Emitter;
} // namespace Atrum::Visual::Particle

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
namespace Atrum::ParticleEditor {
struct UiState;
} // namespace Atrum::ParticleEditor
#endif

/// パーティクル JSON の編集・プレビュー専用シーン（Development + ImGui）
class SceneParticleEditor : public Scene {
public:
  SceneParticleEditor();
  ~SceneParticleEditor() override;

  void EnterScene() override;
  void Update() override;
  void ExitScene() override;

  static SceneParticleEditor *Instance();

  /// 戦闘カメラ固定は使わず DebugCamera で回転する
  bool IsStageViewCameraLocked() const override { return false; }

protected:
  void DrawScene() override;

private:
  std::unique_ptr<Atrum::Visual::Particle::Emitter> previewEmitter_{};

  std::vector<Atrum::Visual::Particle::Param> layerParams_{};

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  std::unique_ptr<Atrum::ParticleEditor::UiState> uiState_{};
#endif

  void SetupPreviewCamera();
  void LoadFromJsonPath(const std::string &jsonFilePath);
  void SaveToJsonPath(const std::string &jsonFilePath);
  void ApplyPreview();
  void HandleEditorRequests();
};
