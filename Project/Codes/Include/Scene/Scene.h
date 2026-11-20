#pragma once

#include "Camera/StageViewFramingBounds.h"
#include "Scene/SceneCameraRig.h"
#include "System/Particle/EmitterManager.h"
#include <memory>
#include <vector>

namespace Atrum {

class Entity;
class Camera;

/// <summary>
/// シーン類 > 基底クラス
/// </summary>
class Scene {

protected:
  SceneCameraRig cameraRig_{};

  StageViewFramingBounds stageViewFramingBounds_{
      StageViewFramingBounds::DefaultFromStageConstants()};

  /// このシーンに紐づくパーティクル（EnterScene で生成、ExitScene で解放）
  std::unique_ptr<Particle::Emitter::EmitterManager> particleEmitterManager_{};

  /// 未生成なら EmitterManager を構築する（EnsureCommonResources から呼ぶ）
  void EnsureParticleEmitterManager();

  /// 紐づいたパーティクルマネージャを破棄する（ReleaseCommonResources から呼ぶ）
  void ReleaseParticleEmitterManager();

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

  Atrum::Camera *GetCamera();
  const Atrum::Camera *GetCamera() const;

  /// Update 完了後・Draw 直前（GameOrder / ActiveCameraSystem から呼ぶ）
  void PrepareCamerasForDraw();

  /// JSON 実体など rig 外のカメラ候補（未使用シーンは nullptr）
  virtual const std::vector<std::unique_ptr<Entity>> *
  GetSupplementalCameraEntities() const {
    return nullptr;
  }

  /// <summary>
  /// true のとき DebugCamera の操作を行わず EnterScene で設定した戦闘カメラを維持する。
  /// 本編・タイトル・リザルトなどプレイヤー向けシーンは override で true 必須
  /// （方針: EditorAssets/Tools/CursorAI/docs/feature-memos/player_friendly_camera_policy.md）
  /// </summary>
  virtual bool IsStageViewCameraLocked() const { return false; }

  void SetStageViewFramingBounds(const StageViewFramingBounds &bounds) {
    stageViewFramingBounds_ = bounds;
  }

  const StageViewFramingBounds &GetStageViewFramingBounds() const {
    return stageViewFramingBounds_;
  }

  /// <summary>
  /// false のとき DebugCamera は行列のみ更新（本編・未設定シーンの既定）。
  /// true は制作ツール用シーンでのみ override（入力可否の組み合わせは
  /// SceneDebugCameraInputPolicy を参照）。
  /// </summary>
  virtual bool AllowDebugCameraControl() const { return false; }

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
