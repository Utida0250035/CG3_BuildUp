#pragma once

#include "Camera/Camera.h"
#include <memory>

/// <summary>
/// シーン類 > 基底クラス
/// </summary>
class Scene {

protected:
  /// <summary>
  /// シーンごとのカメラ
  /// </summary>
  std::unique_ptr<Atrum::Camera> camera_;

  void InitializeCamera();

  /// <summary>
  /// 戦闘ステージ全体が画面に収まるようカメラを配置する
  /// </summary>
  void SetupStageViewCamera();

  /// <summary>
  /// シーン固有の描画（PreDraw と PostDraw の間）
  /// </summary>
  virtual void DrawScene() = 0;

public:
  Scene();
  virtual ~Scene();

  Atrum::Camera *GetCamera() { return camera_.get(); }
  const Atrum::Camera *GetCamera() const { return camera_.get(); }

  /// <summary>
  /// true のとき DebugCamera の操作を行わず EnterScene で設定した戦闘カメラを維持する
  /// </summary>
  virtual bool IsStageViewCameraLocked() const { return false; }

  /// <summary>
  /// シーン進入関数 純粋仮想
  /// </summary>
  virtual void EnterScene() = 0;

  /// <summary>
  /// 更新関数 純粋仮想
  /// </summary>
  virtual void Update() = 0;

  /// <summary>
  /// 描画関数（PreDraw → シーン描画 → 遷移演出 → PostDraw）
  /// </summary>
  void Draw();

  /// <summary>
  /// シーン退出関数 純粋仮想
  /// </summary>
  virtual void ExitScene() = 0;
};
