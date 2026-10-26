#pragma once

#include "System/Particle/ParticleParam.h"
#include <cstdint>
#include <string>
#include <vector>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Visual::Particle {
class Emitter;
} // namespace Atrum::Visual::Particle

namespace Atrum::ParticleEditor {

/// ImGui パーティクルエディタの状態（シーンが保持）
struct UiState {
  std::string jsonFilePath = "../EditorAssets/Particles/editorTest.json";
  int selectedLayerIndex = 0;
  bool requestApplyPreview = false;
  bool requestReloadFromDisk = false;
  bool requestSaveToDisk = false;
  bool requestReplay = false;
};

/// メインウィンドウ。操作フラグを立て、Param を直接編集する
void Draw(UiState &uiState,
          std::vector<Atrum::Visual::Particle::Param> &layerParams);

} // namespace Atrum::ParticleEditor

#endif
