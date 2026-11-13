#pragma once

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

  bool IsStageViewCameraLocked() const override { return true; }

  const std::string &SceneJsonPath() const { return sceneJsonPath_; }

protected:
  void DrawScene() override;

private:
  std::string sceneJsonPath_{};
  std::string sceneDisplayName_{};
  Scene *returnScene_ = nullptr;
  std::vector<std::unique_ptr<Entity>> entities_{};
  // Release シーン巡回の対象なら左上に「次のシーンへ」案内を描く
  bool showsNextSceneHint_ = false;

  void TeardownEntities();
  void SetupRuntimeLighting();
  void DrawRuntimeEntities();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  void DrawRuntimeViewContent(int width, int height);
#endif
};

/// 非シングルトン SceneBuildUp の生成・所有（遷移先ポインタはここが保持）
class SceneBuildUpRegistry final {
public:
  ~SceneBuildUpRegistry();

  static SceneBuildUpRegistry &Instance();

  /// 新しい SceneBuildUp を生成し、シーン遷移用の生ポインタを返す
  Scene *Launch(const std::string &sceneJsonPath, Scene *returnScene);

  /// ExitScene 完了後に所有権を解放
  void OnSceneExited(SceneBuildUp *scene);

private:
  std::vector<std::unique_ptr<SceneBuildUp>> ownedScenes_{};
};

} // namespace Atrum
