#pragma once

#include "Camera/Camera.h"
#include "System/Particle/EmitterManager.h"
#include <memory>

namespace Atrum {

/// <summary>
/// シーン類 > 基底クラス
/// </summary>
class Scene {

protected:
  /// <summary>
  /// シーンごとのカメラ
  /// </summary>
  std::unique_ptr<Atrum::Camera> camera_;

  /// このシーンに紐づくパーティクル（EnterScene で生成、ExitScene で解放）
  std::unique_ptr<Particle::Emitter::EmitterManager> particleEmitterManager_{};

  /// 未生成なら EmitterManager を構築する（EnsureCommonResources から呼ぶ）
  void EnsureParticleEmitterManager();

  /// 紐づいたパーティクルマネージャを破棄する（ReleaseCommonResources から呼ぶ）
  void ReleaseParticleEmitterManager();

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

  /// EnterScene 先頭で呼ぶ。シーン共通リソース（パーティクル等）を確保する
  void EnsureCommonResources();

  Particle::Emitter::EmitterManager *GetParticleEmitterManager();

  const Particle::Emitter::EmitterManager *GetParticleEmitterManager() const {
    return particleEmitterManager_.get();
  }

  /// ExitScene でシーン共通リソースを解放する
  void ReleaseCommonResources();

  Atrum::Camera *GetCamera() { return camera_.get(); }
  const Atrum::Camera *GetCamera() const { return camera_.get(); }

  /// <summary>
  /// true のとき DebugCamera の操作を行わず EnterScene で設定した戦闘カメラを維持する
  /// </summary>
  virtual bool IsStageViewCameraLocked() const { return false; }

  /// false のとき DebugCamera は行列のみ更新（エディットビュー外など）
  virtual bool AllowDebugCameraControl() const { return true; }

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
  virtual void Draw();

  /// <summary>
  /// シーン退出関数 純粋仮想
  /// </summary>
  virtual void ExitScene() = 0;
};

} // namespace Atrum
