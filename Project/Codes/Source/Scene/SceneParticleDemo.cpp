#include "Scene/SceneParticleDemo.h"

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/LightData.h"
#include "Input/PlayInput.h"
#include "Math/Transform.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneEntityEditor.h"
#include "Scene/SceneParticleEditor.h"
#include "System/DeltaTime.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/Editor/Particle/ParticleEditorReferenceGrid.h"
#include "System/Particle/ParticleEmitter.h"
#include "System/StageBounds.h"
#include "System/WhiteTexture.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#endif

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>

namespace {

enum class DemoStepKind { BallWallArena, ParticleShowcase };

struct DemoStep {
  const char *titleJa;
  const char *subtitleJa;
  DemoStepKind kind;
  const char *jsonRelative;
  float durationSeconds;
};

constexpr float kBallArenaLeftWallX = -5.0f;
constexpr float kBallArenaRightWallX = 5.0f;
constexpr float kBallArenaWallHeight = 5.5f;

const DemoStep kDemoProgram[] = {
    {"ボール×壁＋衝撃波",
     "ボールが左右の壁と床で跳ね返り、衝突位置でパーティクル",
     DemoStepKind::BallWallArena, "demos/ball_wall_shockwave.json", 22.0f},
    {"01 噴水バースト", "BURST / POINT / DIRECTIONAL",
     DemoStepKind::ParticleShowcase,
     "samples/01_burst_directional_fountain.json", 5.0f},
    {"02 球状爆発", "SPHERE / RADIAL_SPREAD",
     DemoStepKind::ParticleShowcase,
     "samples/02_burst_sphere_radial_spread.json", 5.0f},
    {"03 収束閃光", "RADIAL_CONVERGE",
     DemoStepKind::ParticleShowcase, "samples/03_burst_radial_converge.json",
     5.5f},
    {"04 連続噴水", "CONTINUOUS",
     DemoStepKind::ParticleShowcase, "samples/04_continuous_fountain.json",
     5.0f},
    {"05 環状ループ", "CIRCLE_XZ / isLooping",
     DemoStepKind::ParticleShowcase,
     "samples/05_continuous_circle_xz_loop.json", 5.0f},
    {"06 箱煙", "AABB 発生",
     DemoStepKind::ParticleShowcase, "samples/06_emit_aabb_smoke_box.json",
     5.0f},
    {"07 水平円盤", "CIRCLE_XY",
     DemoStepKind::ParticleShowcase, "samples/07_emit_circle_xy_disk.json",
     5.0f},
    {"08 縦円盤", "CIRCLE_YZ",
     DemoStepKind::ParticleShowcase, "samples/08_emit_circle_yz_wall.json",
     5.0f},
    {"09 ビルボード", "BILLBOARD",
     DemoStepKind::ParticleShowcase, "samples/09_render_billboard.json",
     5.0f},
    {"10 モデル粒子", "MODEL",
     DemoStepKind::ParticleShowcase, "samples/10_render_model.json", 5.0f},
    {"11 キル球内", "killField INSIDE",
     DemoStepKind::ParticleShowcase, "samples/11_killfield_inside_sphere.json",
     5.5f},
    {"12 キル箱外", "killField OUTSIDE",
     DemoStepKind::ParticleShowcase, "samples/12_killfield_outside_aabb.json",
     5.5f},
    {"13 減衰浮遊", "damping",
     DemoStepKind::ParticleShowcase, "samples/13_damping_float.json", 5.5f},
    {"14 閃光＋煙", "多レイヤー",
     DemoStepKind::ParticleShowcase, "samples/14_multilayer_flash_smoke.json",
     5.5f},
    {"15 LOCAL バースト", "エミッター追従座標",
     DemoStepKind::ParticleShowcase, "samples/15_simulation_local_burst.json",
     5.5f},
};

constexpr int kDemoStepCount =
    static_cast<int>(sizeof(kDemoProgram) / sizeof(kDemoProgram[0]));

std::string ParticleJsonPath(const char *relativePath) {
  return std::string(Atrum::EditorAssets::kParticlesDirectory) + "/" +
         relativePath;
}

void SetupDemoLighting() {
  Atrum::Math::Vector3 lightDirection{-0.35f, -0.9f, 0.2f};
  lightDirection.Normalize();
  const Atrum::DirectionalLightData directionalLight{
      .color = Atrum::Math::Vector3{1.0f, 1.0f, 1.0f},
      .direction = lightDirection,
      .intensity = 6.0f,
      .lightModel = Atrum::LightModel::HalfLambert,
  };
  ::Atrum::EngineInstance()->SetDirectionalLightData(directionalLight);
}

} // namespace

namespace Atrum {

SceneParticleDemo::SceneParticleDemo() = default;

SceneParticleDemo::~SceneParticleDemo() = default;

SceneParticleDemo *SceneParticleDemo::Instance() {
  static SceneParticleDemo instance;
  return &instance;
}

void SceneParticleDemo::SetupDemoCamera() {
  if (Atrum::Camera *camera = GetCamera()) {
    camera->SetTranslate(Atrum::Math::Vector3{0.0f, 1.8f, -8.5f});
    camera->UpdateMatrix();
  }
}

void SceneParticleDemo::ClearShowcaseEmitter() {
  if (Particle::Emitter::EmitterManager *mgr = GetParticleEmitterManager()) {
    mgr->RemoveByName(kShowcaseEmitterName);
  }
}

void SceneParticleDemo::SpawnWallImpact(const Math::Vector3 &worldPosition) {
  Particle::Emitter::EmitterManager *mgr = GetParticleEmitterManager();
  if (mgr == nullptr || impactJsonPath_.empty()) {
    return;
  }
  if (!std::filesystem::exists(impactJsonPath_)) {
    return;
  }

  const std::string emitterName =
      "particle_demo_impact_" + std::to_string(++impactSequence_);
  mgr->AddOneShotEmitter(emitterName, impactJsonPath_.c_str(), worldPosition);
}

void SceneParticleDemo::BeginStep(const int stepIndex) {
  stepIndex_ = (stepIndex % kDemoStepCount + kDemoStepCount) % kDemoStepCount;
  stepElapsedSeconds_ = 0.0f;
  wallImpactCooldownSeconds_ = 0.0f;

  const DemoStep &step = kDemoProgram[static_cast<size_t>(stepIndex_)];
  impactJsonPath_ = ParticleJsonPath(step.jsonRelative);

  ClearShowcaseEmitter();

  Particle::Emitter::EmitterManager *mgr = GetParticleEmitterManager();
  if (mgr == nullptr) {
    return;
  }
  mgr->SetCamera(GetCamera());
  mgr->RemoveInactive();

  if (step.kind == DemoStepKind::ParticleShowcase) {
    namespace fs = std::filesystem;
    if (!fs::exists(impactJsonPath_)) {
      return;
    }

    auto emitter = std::make_unique<Visual::Particle::Emitter>();
    emitter->SetName(kShowcaseEmitterName);
    emitter->SetCamera(GetCamera());
    emitter->Initialize(impactJsonPath_.c_str());
    mgr->Register(kShowcaseEmitterName, std::move(emitter));
    mgr->SetRetainInactiveByName(kShowcaseEmitterName, true);
    if (Visual::Particle::Emitter *live = mgr->Find(kShowcaseEmitterName)) {
      live->RestartPreview();
    }
  } else if (step.kind == DemoStepKind::BallWallArena) {
    ballPosition_ = {0.0f, 0.0f, 0.0f};
    ballVelocity_ = {7.0f, 5.5f, 0.0f};
  }
}

void SceneParticleDemo::AdvanceStep(const int delta) {
  BeginStep(stepIndex_ + delta);
}

void SceneParticleDemo::UpdateBallArenaStep(const float deltaSeconds) {
  wallImpactCooldownSeconds_ =
      std::max(0.0f, wallImpactCooldownSeconds_ - deltaSeconds);

  ballPosition_ += ballVelocity_ * deltaSeconds;
  ballVelocity_.y += -14.0f * deltaSeconds;

  const float groundY = kGroundHeight + ballRadius_;
  if (ballPosition_.y <= groundY) {
    ballPosition_.y = groundY;
    if (ballVelocity_.y < 0.0f) {
      ballVelocity_.y = -ballVelocity_.y * 0.88f;
      if (std::abs(ballVelocity_.y) > 1.2f && wallImpactCooldownSeconds_ <= 0.0f) {
        SpawnWallImpact({ballPosition_.x, groundY, ballPosition_.z});
        wallImpactCooldownSeconds_ = 0.12f;
      }
    }
  }

  const float leftLimit = kBallArenaLeftWallX + ballRadius_;
  const float rightLimit = kBallArenaRightWallX - ballRadius_;
  if (ballPosition_.x <= leftLimit && ballVelocity_.x < 0.0f) {
    ballPosition_.x = leftLimit;
    ballVelocity_.x = std::abs(ballVelocity_.x) * 0.94f;
    if (wallImpactCooldownSeconds_ <= 0.0f) {
      SpawnWallImpact(
          {kBallArenaLeftWallX, ballPosition_.y, ballPosition_.z});
      wallImpactCooldownSeconds_ = 0.12f;
    }
  } else if (ballPosition_.x >= rightLimit && ballVelocity_.x > 0.0f) {
    ballPosition_.x = rightLimit;
    ballVelocity_.x = -std::abs(ballVelocity_.x) * 0.94f;
    if (wallImpactCooldownSeconds_ <= 0.0f) {
      SpawnWallImpact(
          {kBallArenaRightWallX, ballPosition_.y, ballPosition_.z});
      wallImpactCooldownSeconds_ = 0.12f;
    }
  }
}

void SceneParticleDemo::UpdateShowcaseStep(const float /*deltaSeconds*/) {
  if (Particle::Emitter::EmitterManager *mgr = GetParticleEmitterManager()) {
    if (Visual::Particle::Emitter *emitter = mgr->Find(kShowcaseEmitterName)) {
      emitter->SetCamera(GetCamera());
    }
  }
}

void SceneParticleDemo::EnterScene() {
  EnsureCommonResources();
  SetupDemoCamera();
  impactSequence_ = 0;
  autoAdvance_ = true;
  BeginStep(0);
}

void SceneParticleDemo::Update() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F1)) {
    CommandChangeScene::Instance()->Set(
        SceneEntityEditor::Instance(),
        std::make_unique<ChangeSceneFade>().release());
    return;
  }
  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F2)) {
    CommandChangeScene::Instance()->Set(
        SceneParticleEditor::Instance(),
        std::make_unique<ChangeSceneFade>().release());
    return;
  }
#endif

  const float deltaSeconds = ::Atrum::EngineFrameDeltaTime()->GetDeltaTime();
  stepElapsedSeconds_ += deltaSeconds;

  const DemoStep &step = kDemoProgram[static_cast<size_t>(stepIndex_)];
  if (step.kind == DemoStepKind::BallWallArena) {
    UpdateBallArenaStep(deltaSeconds);
  } else {
    UpdateShowcaseStep(deltaSeconds);
  }

  if (autoAdvance_ && stepElapsedSeconds_ >= step.durationSeconds) {
    AdvanceStep(1);
  }

  Particle::Emitter::EmitterManager *mgr = GetParticleEmitterManager();
  if (mgr != nullptr) {
    mgr->SetCamera(GetCamera());
    mgr->Update();
    mgr->RemoveInactive();
  }

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (ImGui::Begin("Particle Demo Showreel")) {
    ImGui::TextUnformatted(
        "F1: Entity Editor  |  F2: Particle Editor  |  Space: 次の項目");
    ImGui::Checkbox("Auto advance", &autoAdvance_);
  const float remaining =
        std::max(0.0f, step.durationSeconds - stepElapsedSeconds_);
    ImGui::Text("%d / %d  %.1fs left", stepIndex_ + 1, kDemoStepCount,
                remaining);
    ImGui::Separator();
    ImGui::TextUnformatted(step.titleJa);
    ImGui::TextDisabled("%s", step.subtitleJa);
    ImGui::TextDisabled("%s", impactJsonPath_.c_str());
    if (ImGui::Button("Prev")) {
      AdvanceStep(-1);
    }
    ImGui::SameLine();
    if (ImGui::Button("Next")) {
      AdvanceStep(1);
    }
    ImGui::SameLine();
    if (ImGui::Button("Replay step")) {
      BeginStep(stepIndex_);
    }
  }
  ImGui::End();

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::SPACE)) {
    AdvanceStep(1);
  }
#endif
}

void SceneParticleDemo::DrawArenaProps() {
  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  const Math::Vector4 wallColor{0.55f, 0.58f, 0.65f, 1.0f};
  const float y0 = kGroundHeight;
  const float y1 = kGroundHeight + kBallArenaWallHeight;

  engine->DrawLine3D(WhiteTextureIndex(), wallColor, Math::Transform{},
                     {kBallArenaLeftWallX, y0, 0.0f},
                     {kBallArenaLeftWallX, y1, 0.0f}, 0.08f);
  engine->DrawLine3D(WhiteTextureIndex(), wallColor, Math::Transform{},
                     {kBallArenaRightWallX, y0, 0.0f},
                     {kBallArenaRightWallX, y1, 0.0f}, 0.08f);

  const Math::Vector4 floorColor{0.35f, 0.38f, 0.42f, 0.85f};
  engine->DrawLine3D(WhiteTextureIndex(), floorColor, Math::Transform{},
                     {kBallArenaLeftWallX - 0.5f, kGroundHeight, 0.0f},
                     {kBallArenaRightWallX + 0.5f, kGroundHeight, 0.0f},
                     0.06f);
}

void SceneParticleDemo::DrawDemoBall() {
  const DemoStep &step = kDemoProgram[static_cast<size_t>(stepIndex_)];
  if (step.kind != DemoStepKind::BallWallArena) {
    return;
  }

  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  Math::Transform transform{};
  transform.translate = ballPosition_;
  transform.scale = Math::Vector3::One() * ballRadius_;
  const Math::Matrix4x4 worldMatrix = transform.MakeWorldMatrix();
  const Math::Vector4 ballColor{0.35f, 0.78f, 1.15f, 1.0f};
  engine->DrawSphere(WhiteTextureIndex(), ballColor, Math::Transform{},
                     worldMatrix, ballRadius_, 10, true);
}

void SceneParticleDemo::DrawScene() {
  SetupDemoLighting();

  Atrum::Engine *engine = ::Atrum::EngineInstance();
  Particle::Emitter::EmitterManager *mgr = GetParticleEmitterManager();
  if (engine == nullptr || mgr == nullptr) {
    return;
  }

  engine->SetCamera(GetCamera());

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  DrawParticleEditorReferenceGrid(engine);
#endif

  const DemoStep &step = kDemoProgram[static_cast<size_t>(stepIndex_)];
  if (step.kind == DemoStepKind::BallWallArena) {
    DrawArenaProps();
    DrawDemoBall();
  }

  engine->SetBlendModeForFrame(BlendMode::ADD);
  mgr->Draw();
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

void SceneParticleDemo::ExitScene() {
  ClearShowcaseEmitter();
  if (Particle::Emitter::EmitterManager *mgr = GetParticleEmitterManager()) {
    mgr->Clear();
  }
  ReleaseCommonResources();
}

} // namespace Atrum
