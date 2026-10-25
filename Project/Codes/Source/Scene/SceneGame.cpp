#include "Scene/SceneGame.h"

#include "Camera/CameraController/CameraFocusPushController.h"
#include "Camera/CameraController/CameraGameIntroController.h"
#include "Camera/CombatCameraShake.h"
#include "Camera/PlayerDamageScreenVignette.h"
#include "Scene/BossDeathPresentation.h"
#include "Scene/PlayerDeathPresentation.h"
#include "Input/AbstractInput.h"
#include "Scene/SceneGameBattleHud.h"
#include "Scene/SceneScreenUi.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Resource/LightData.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneResult.h"
#include "System/BossCombatCleanup.h"
#include "System/OrbiterChildUtils.h"
#include "System/CmpSkyDome.h"
#include "System/CmpParry.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"
#include "System/DeltaTime.h"
#include "System/EntityFactory.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/PlaySe.h"

namespace {

using Atrum::Engine;

void SetupGameLighting() {
  Atrum::Math::Vector3 lightDirection{-0.4f, -0.85f, 0.15f};
  lightDirection.Normalize();
  const Atrum::DirectionalLightData directionalLight{
      .color = Atrum::Math::Vector3{1.0f, 1.0f, 1.0f},
      .direction = lightDirection,
      .intensity = 8.0f,
      .lightModel = Atrum::LightModel::HalfLambert,
  };
  Engine::Instance()->SetDirectionalLightData(directionalLight);
}

} // namespace

SceneGame::SceneGame() {}

SceneGame *SceneGame::Instance() {
  static SceneGame instance;
  return &instance;
}

void SceneGame::EnterScene() {
  pendingDefeat_ = DefeatKind::None;
  bossDeathResultTransitionRequested_ = false;
  bossDeathPresentation_.reset();
  bossDeathCameraController_.reset();
  playerDeathPresentation_.reset();
  playerDeathRestartTransitionRequested_ = false;
  SpawnEntities();

  introCameraController_ =
      std::make_unique<Atrum::CameraGameIntroController>();
  introCameraController_->Initialize();
  introCameraController_->SetCamera(GetCamera());
  if (boss_) {
    introCameraController_->BeginIntro(boss_->GetWorldPosition());
  } else {
    SetupStageViewCamera();
  }

  Atrum::Particle::Combat::SetEffectCamera(GetCamera());
  Atrum::CombatCameraShake::Instance().Reset();
  Atrum::PlayerDamageScreenVignette::Instance().Reset();
  Atrum::SceneScreenUi::PreloadGameIntroUiTextures();
  sceneBgmHandle_ = Atrum::PlayBgmLoop({"./Assets/Audios/bgmGame.mp3",
                                        "./Resources/bgmGame.mp3"});
}

void SceneGame::ExitScene() {
  Atrum::StopSe(sceneBgmHandle_);
  sceneBgmHandle_ = {};
  introCameraController_.reset();
  bossDeathCameraController_.reset();
  ClearEntities();
  Atrum::ResetBossCombatSessionState();
  Atrum::Particle::Combat::ClearCombatParticleEffects();
}

void SceneGame::Update() {
  if (pendingDefeat_ == DefeatKind::Boss) {
    if (bossDeathPresentation_) {
      UpdateBossDeathPresentation();
    } else if (!bossDeathResultTransitionRequested_) {
      bossDeathResultTransitionRequested_ = true;
      RequestTransitionToResult();
    }
    return;
  }

  if (pendingDefeat_ == DefeatKind::Player) {
    if (playerDeathPresentation_) {
      UpdatePlayerDeathPresentation();
    } else if (!playerDeathRestartTransitionRequested_) {
      playerDeathRestartTransitionRequested_ = true;
      RequestRestartRound();
    }
    return;
  }

  if (pendingDefeat_ != DefeatKind::None) {
    return;
  }

  if (Atrum::CommandChangeScene::Instance()->GetIsExecute()) {
    return;
  }

  if (introCameraController_ && !introCameraController_->IsFinished()) {
    if (Atrum::AbstractInput::Instance()->GetData().isSubmitTrigger) {
      introCameraController_->SkipIntro();
    } else {
      introCameraController_->Update();
    }
    UpdateIntroPresentation();
    Atrum::Particle::Combat::SetEffectCamera(GetCamera());
    return;
  }

  UpdateEntities();

  if (Atrum::FrameDeltaTime *frameDeltaTime = Atrum::FrameDeltaTime::Instance()) {
    Atrum::CombatCameraShake &cameraShake =
        Atrum::CombatCameraShake::Instance();
    const float deltaTime = frameDeltaTime->GetDeltaTime();
    cameraShake.Update(deltaTime);
    Atrum::PlayerDamageScreenVignette::Instance().Update(deltaTime);
    cameraShake.ApplyStageBattleView(GetCamera());
  }

  Atrum::Particle::Combat::SetEffectCamera(GetCamera());
  Atrum::Particle::Combat::UpdateCombatParticleEffects();
  CheckRoundEnd();
}

void SceneGame::DrawScene() {
  SetupGameLighting();
  DrawEntities();
  Atrum::Particle::Combat::DrawCombatParticleEffects();
  DrawBattleHud();
  DrawIntroScreenUi();
}

void SceneGame::DrawIntroScreenUi() {
  if (introCameraController_ == nullptr ||
      introCameraController_->IsFinished()) {
    return;
  }

  Atrum::AtrumEngine *engine = Atrum::AtrumEngine::Instance();
  if (engine == nullptr || GetCamera() == nullptr) {
    return;
  }

  Atrum::SceneScreenUi::DrawGameIntroHowToNextUi(engine, GetCamera());
}

void SceneGame::DrawBattleHud() {
  if (!player_ || !boss_) {
    return;
  }

  if (!player_->HasUpdCmp<Atrum::CmpHealthHit>() ||
      !boss_->HasUpdCmp<Atrum::CmpHealthHit>()) {
    return;
  }

  Atrum::Engine *engine = Atrum::Engine::Instance();
  if (engine == nullptr || GetCamera() == nullptr) {
    return;
  }

  // 3D 描画後もスプライト用にカメラを再設定する
  engine->SetCamera(GetCamera());

  Atrum::PlayerDamageScreenVignette::Instance().Draw(engine);

  Atrum::CmpHealthHit *playerHealth = player_->GetUpdCmp<Atrum::CmpHealthHit>();
  Atrum::CmpHealthHit *bossHealth = boss_->GetUpdCmp<Atrum::CmpHealthHit>();

  const float bossHpBarReveal =
      introCameraController_ ? introCameraController_->GetBossHpBarReveal01()
                             : 1.0f;

  Atrum::DrawSceneGameBattleHud(
      engine, playerHealth->GetHealthCount(),
      playerHealth->GetInitialHealthCount(), bossHealth->GetHealthCount(),
      bossHealth->GetInitialHealthCount(), bossHpBarReveal);
}

void SceneGame::SpawnEntities() {
  ClearEntities();

  skyDome_ = Atrum::EntityFactory::SkyDome(GetCamera());
  backgrounds_ = Atrum::EntityFactory::ScrollSpheres();
  foregroundGroundClouds_ = Atrum::EntityFactory::ForegroundGroundClouds();
  player_ = Atrum::EntityFactory::PlayerParry();
  boss_ = Atrum::EntityFactory::EnemyAstro(player_.get());
}

void SceneGame::ClearEntities() {
  if (player_) {
    if (Atrum::CmpParry *cmpParry = player_->GetDataCmp<Atrum::CmpParry>()) {
      if (cmpParry->HasEntParry()) {
        cmpParry->RefEntParry().Finalize();
      }
    }
    player_->Finalize();
    player_.reset();
  }

  if (boss_) {
    boss_->Finalize();
    boss_.reset();
  }

  if (skyDome_) {
    skyDome_->Finalize();
    skyDome_.reset();
  }

  for (std::unique_ptr<Atrum::Entity> &background : backgrounds_) {
    if (background) {
      background->Finalize();
    }
  }
  backgrounds_.clear();

  for (std::unique_ptr<Atrum::Entity> &cloud : foregroundGroundClouds_) {
    if (cloud) {
      cloud->Finalize();
    }
  }
  foregroundGroundClouds_.clear();
}

void SceneGame::UpdateBackgrounds() {
  for (std::unique_ptr<Atrum::Entity> &background : backgrounds_) {
    if (background) {
      background->Update();
    }
  }
}

void SceneGame::UpdatePlayerDeathPresentation() {
  if (!playerDeathPresentation_) {
    return;
  }

  Atrum::FrameDeltaTime *frameDeltaTime = Atrum::FrameDeltaTime::Instance();
  if (frameDeltaTime == nullptr) {
    return;
  }

  playerDeathPresentation_->Update(frameDeltaTime->GetDeltaTime());

  UpdateBackgrounds();

  Atrum::Particle::Combat::SetEffectCamera(GetCamera());
  Atrum::Particle::Combat::UpdateCombatParticleEffects();

  if (playerDeathPresentation_->IsFinished() &&
      !playerDeathRestartTransitionRequested_) {
    playerDeathRestartTransitionRequested_ = true;
    RequestRestartRound();
  }
}

void SceneGame::UpdateBossDeathPresentation() {
  if (!bossDeathPresentation_) {
    return;
  }

  Atrum::FrameDeltaTime *frameDeltaTime = Atrum::FrameDeltaTime::Instance();
  if (frameDeltaTime == nullptr) {
    return;
  }

  if (bossDeathCameraController_ && boss_) {
    bossDeathCameraController_->SetFocusWorldPosition(
        boss_->GetWorldPosition());
    bossDeathCameraController_->Update();
  }

  bossDeathPresentation_->Update(frameDeltaTime->GetDeltaTime());

  UpdateBackgrounds();

  Atrum::Particle::Combat::SetEffectCamera(GetCamera());
  Atrum::Particle::Combat::UpdateCombatParticleEffects();

  if (bossDeathPresentation_->IsFinished() &&
      !bossDeathResultTransitionRequested_) {
    bossDeathResultTransitionRequested_ = true;
    RequestTransitionToResult();
  }
}

void SceneGame::UpdateIntroPresentation() {
  UpdateBackgrounds();

  if (boss_) {
    Atrum::OrbiterChildUtils::UpdateBossOrbiterMovementOnly(*boss_);
  }
}

void SceneGame::UpdateEntities() {
  UpdateBackgrounds();

  // 自機のパリィ有効化（INPUT/MOVEMENT）を先に済ませてからボス弾の当たり判定を行う
  if (player_) {
    player_->Update();
  }

  if (boss_) {
    boss_->Update();
    if (Atrum::CmpReflectAbleBullet *cmpReflectAbleBullet =
            boss_->GetUpdCmp<Atrum::CmpReflectAbleBullet>()) {
      cmpReflectAbleBullet->LateResolveCombatHits();
    }
  }
}

void SceneGame::DrawEntities() {
  if (skyDome_) {
    if (skyDome_->HasDrawCmp<Atrum::CmpSkyDome>()) {
      skyDome_->GetDrawCmp<Atrum::CmpSkyDome>()->SetCamera(GetCamera());
    }
    skyDome_->Draw();
  }

  for (const std::unique_ptr<Atrum::Entity> &background : backgrounds_) {
    background->Draw();
  }

  if (player_) {
    player_->Draw();
  }

  if (boss_) {
    boss_->Draw();
  }

  for (const std::unique_ptr<Atrum::Entity> &cloud : foregroundGroundClouds_) {
    if (cloud) {
      cloud->Draw();
    }
  }
}

void SceneGame::CheckRoundEnd() {
  if (!player_ || !boss_) {
    return;
  }

  Atrum::CmpHealthHit *bossHealth = boss_->GetUpdCmp<Atrum::CmpHealthHit>();
  Atrum::CmpHealthHit *playerHealth = player_->GetUpdCmp<Atrum::CmpHealthHit>();
  if (!bossHealth || !playerHealth) {
    return;
  }

  if (bossHealth->GetHealthCount() == 0) {
    pendingDefeat_ = DefeatKind::Boss;
    OnBossDefeated();
    return;
  }

  if (playerHealth->GetHealthCount() == 0) {
    pendingDefeat_ = DefeatKind::Player;
    OnPlayerDefeated();
  }
}

void SceneGame::RequestTransitionToResult() {
  Atrum::CommandChangeScene::Instance()->Set(SceneResult::Instance(),
                                             new Atrum::ChangeSceneFade());
}

void SceneGame::RequestRestartRound() {
  // 自機撃墜時は同シーンへ遷移し EnterScene で再生成する
  Atrum::CommandChangeScene::Instance()->Set(SceneGame::Instance(),
                                             new Atrum::ChangeSceneFade());
}

void SceneGame::OnBossDefeated() {
  Atrum::StopSe(sceneBgmHandle_);
  sceneBgmHandle_ = {};

  if (!boss_) {
    RequestTransitionToResult();
    return;
  }

  bossDeathCameraController_ =
      std::make_unique<Atrum::CameraFocusPushController>();
  bossDeathCameraController_->BeginBossDefeatFromStageBattle(
      GetCamera(), boss_->GetWorldPosition());

  bossDeathPresentation_ = std::make_unique<Atrum::BossDeathPresentation>();
  bossDeathPresentation_->Begin(boss_.get());
}

void SceneGame::OnPlayerDefeated() {
  if (!player_) {
    RequestRestartRound();
    return;
  }

  playerDeathPresentation_ = std::make_unique<Atrum::PlayerDeathPresentation>();
  playerDeathPresentation_->Begin(player_.get());
}
