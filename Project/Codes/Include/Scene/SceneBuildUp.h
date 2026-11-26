#pragma once

#include "Audio/AudioHandle.h"
#include "Scene/SceneBuildUpAllowDebugCameraContract.h"
#include "Scene.h"
#include <memory>
#include <string>
#include <vector>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
namespace Atrum::Editor {
struct SubViewportRect;
}
#endif

namespace Atrum {

class Entity;
class Scene;

/// エディタ JSON シーンを読み込んで実行するランタイムシーン（シングルトンではない）
class SceneBuildUp final : public Scene {
public:
  SceneBuildUp(std::string sceneJsonPath, Scene *returnScene);
  ~SceneBuildUp() override;

  void EnterScene() override;
  void Update() override;
  void Draw() override;
  void ExitScene() override;

  bool IsStageViewCameraLocked() const override;
  // AllowDebugCameraControl は override しない（本番相当のカメラ契約）

  void SetStageViewCameraLocked(bool locked) { stageViewCameraLocked_ = locked; }

  const std::vector<std::unique_ptr<Entity>> *
  GetSupplementalCameraEntities() const override {
    return &entities_;
  }

  const std::string &SceneJsonPath() const { return sceneJsonPath_; }
  const std::string &SceneDisplayName() const { return sceneDisplayName_; }

  void SetReturnScene(Scene *returnScene) { returnScene_ = returnScene; }
  Scene *ReturnScene() const { return returnScene_; }

  bool TryHandleInputWhileSceneTransitionBlocked() override;

  bool SceneDocumentLoadFailed() const { return sceneDocumentLoadFailed_; }
  Audio::Handle &SceneBgmHandle() { return sceneBgmHandle_; }
  Audio::Handle &SceneAmbientLoopHandle() { return sceneAmbientLoopSeHandle_; }

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  void DrawRuntimeViewContent(int width, int height);
#endif

protected:
  void DrawScene() override;

private:
  std::string sceneJsonPath_{};
  std::string sceneDisplayName_{};
  Scene *returnScene_ = nullptr;
  std::vector<std::unique_ptr<Entity>> entities_{};
  bool stageViewCameraLocked_ = true;
  bool sceneDocumentLoadFailed_ = false;
  std::string sceneLoadFailureMessage_{};
  Audio::Handle sceneBgmHandle_{};
  Audio::Handle sceneAmbientLoopSeHandle_{};

  void TeardownEntities();
  void DrawRuntimeEntities();

};

/// 非シングルトン SceneBuildUp の生成・所有（遷移先ポインタはここが保持）
class SceneBuildUpRegistry final {
public:
  ~SceneBuildUpRegistry();

  static SceneBuildUpRegistry &Instance();

  /// SceneBuildUp を生成して所有するのみ（EnterScene・遷移は呼び手／CommandScene が担当）
  Scene *Launch(const std::string &sceneJsonPath, Scene *returnScene);

  /// ExitScene 完了後に所有権を解放
  void OnSceneExited(SceneBuildUp *scene);

private:
  std::vector<std::unique_ptr<SceneBuildUp>> ownedScenes_{};
};

} // namespace Atrum
