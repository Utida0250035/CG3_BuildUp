#pragma once

namespace Atrum {

class AtrumEngine;
class Camera;
class Scene;

/// エンジン側アクティブカメラ解決（P1: primary 1 本）
class ActiveCameraSystem final {
public:
  static ActiveCameraSystem &Instance();

  Camera *GetActiveCamera() const { return activeCamera_; }

  void PrepareSceneAndBindEngine(Scene *scene, AtrumEngine *engine);

  /// GameOrder 後のシーンビュー RT 描画でビューポート Fit 済みカメラを再同期する。
  void SyncEngineAfterSceneViewFit(AtrumEngine *engine, Camera *camera);

private:
  ActiveCameraSystem() = default;

  Camera *activeCamera_ = nullptr;
};

} // namespace Atrum
