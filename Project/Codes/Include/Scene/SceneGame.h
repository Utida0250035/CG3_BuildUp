#pragma once

#include "Audio/AudioHandle.h"
#include "Scene.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace Atrum {

class BossDeathPresentation;
class PlayerDeathPresentation;
class CameraFocusPushController;
class CameraGameIntroController;
class Entity;

/// <summary>
/// ゲームプレイシーン
/// </summary>
class SceneGame : public Scene {
public:
  SceneGame();
  ~SceneGame() override;

  void EnterScene() override;
  void Update() override;
  void ExitScene() override;

  static SceneGame *Instance();

  bool IsStageViewCameraLocked() const override { return true; }

protected:
  void DrawScene() override;

private:
  enum class DefeatKind : uint8_t { None, Boss, Player };

  Atrum::Audio::Handle sceneBgmHandle_{};
  std::unique_ptr<Atrum::Entity> player_;
  std::unique_ptr<Atrum::Entity> boss_;
  std::unique_ptr<Atrum::Entity> skyDome_;
  std::vector<std::unique_ptr<Atrum::Entity>> backgrounds_;
  std::vector<std::unique_ptr<Atrum::Entity>> foregroundGroundClouds_;

  DefeatKind pendingDefeat_ = DefeatKind::None;
  bool bossDeathResultTransitionRequested_ = false;
  bool playerDeathRestartTransitionRequested_ = false;

  std::unique_ptr<Atrum::CameraGameIntroController> introCameraController_;
  std::unique_ptr<Atrum::CameraFocusPushController> bossDeathCameraController_;
  std::unique_ptr<Atrum::BossDeathPresentation> bossDeathPresentation_;
  std::unique_ptr<Atrum::PlayerDeathPresentation> playerDeathPresentation_;

  bool operatorBattleReadyNotified_ = false;
  bool operatorGuidanceDocumentLoaded_ = false;

  // Game→Game 再開時に BGM 位置を戻す（Exit で Get、次 Enter で Set）
  float resumeGameBgmPlaybackSeconds_ = -1.0f;

  /// 戦闘主体（イントロカメラの焦点確定用に先に生成）
  void SpawnCombatEntities();
  /// 背景・SkyDome（イントロカメラ姿勢確定後に生成）
  void SpawnEnvironmentEntities();
  void ClearEntities();
  void UpdateBackgrounds();
  void UpdateEntities();
  /// カメラ演出中：Orbiter の回転のみ更新（攻撃・移動・自機入力は行わない）
  void UpdateIntroPresentation();
  void UpdateBossDeathPresentation();
  void UpdatePlayerDeathPresentation();
  void DrawEntities();
  void DrawBattleHud();
  void DrawIntroScreenUi();
  void DrawHowToPlayUi();
  void DrawOperatorGuidanceUi();
  /// allowRestartPlay が false のときはフェード同期／再開のみ（イントロ中等）
  void MaintainSceneBgmIfNeeded(bool allowRestartPlay = true);

  void CheckRoundEnd();
  void RequestTransitionToResult();
  void RequestRestartRound();

  // 将来: ボス登場・撃破、自機撃墜の演出用フック
  void OnBossDefeated();
  void OnPlayerDefeated();
};

} // namespace Atrum
