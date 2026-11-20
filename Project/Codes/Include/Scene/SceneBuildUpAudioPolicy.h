#pragma once

namespace Atrum {

class SceneBuildUp;

/// BuildUp 検証シーンの BGM／環境ループ SE とフェード遷移同期
struct SceneBuildUpAudioPolicy final {
  static void OnEnterScene(SceneBuildUp &scene);
  static void Tick(SceneBuildUp &scene);
  static void OnExitScene(SceneBuildUp &scene);
};

} // namespace Atrum