#include "System/Particle/ParticleEmitter.h"
#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Renderer/BillboardMatrix.h"
#include "Geometry/GeometryRandom.h"
#include "Math/Lerp.h"
#include "Math/Vector3.h"
#include "System/DeltaTime.h"
#include "System/Json/JsonHandler.h"
#include "System/Random.h"
#include "System/WhiteTexture.h"
#include <cassert>
#include <algorithm>
#include <filesystem>
#include <numbers>

namespace {
namespace Alg = Atrum::Algorithm;
namespace M = Atrum::Math;
namespace G = Atrum::Geometry;
namespace fs = std::filesystem;
} // namespace

namespace Atrum::Visual::Particle {

void Emitter::ConfigureLayerResources(Layer &layer) {
  layer.model = nullptr;
  layer.billboardTextureIndex = WhiteTextureIndex();

  switch (layer.param.renderMode) {
  case RenderMode::MODEL: {
    const std::string objPath =
        layer.param.objDirectoryPath + "/" + layer.param.objFileName;
    const std::string mtlPath =
        layer.param.mtlDirectoryPath + "/" + layer.param.mtlFileName;
    if (fs::exists(objPath) && fs::exists(mtlPath)) {
      layer.model = Engine::Instance()->GetModel(
          layer.param.objDirectoryPath, layer.param.objFileName,
          layer.param.mtlDirectoryPath, layer.param.mtlFileName);
    }
    break;
  }
  case RenderMode::BILLBOARD: {
    if (fs::exists(layer.param.textureFilePath)) {
      layer.billboardTextureIndex =
          Engine::Instance()->GetTexture(layer.param.textureFilePath);
    }
    break;
  }
  case RenderMode::BUILTIN_SPHERE:
  default:
    break;
  }
}

void Emitter::ResetLayerRuntimeState(Layer &layer, const size_t layerIndex) {
  layer.accumulator = 0.0f;
  layer.elapsedSeconds = 0.0f;
  layer.hasBurst = false;
  layer.isFinished = false;

  assert(layer.param.emitRate > 0.0f);
  assert(layer.param.duration > 0.0f);
  assert(layer.param.maxCount > 0);

  particles_[layerIndex].resize(layer.param.maxCount);
  for (Particle &particle : particles_[layerIndex]) {
    particle.isActive = false;
  }
}

void Emitter::InitializeFromParams(const std::vector<Param> &params) {
  frameDeltaTime_ = FrameDeltaTime::Instance();

  layers_.resize(params.size());
  particles_.resize(params.size());

  for (size_t i = 0; i < layers_.size(); ++i) {
    layers_[i].param = params[i];
    ConfigureLayerResources(layers_[i]);
    ResetLayerRuntimeState(layers_[i], i);
  }
}

void Emitter::Initialize(const char *jsonFilePath) {
  std::vector<Param> params{};

  if (fs::exists(jsonFilePath)) {
    params = Json::JsonHandler::LoadFromFile<std::vector<Param>>(jsonFilePath);
  } else {
#if defined(DEVELOPMENT) && !defined(NDEBUG)
    assert(false && "Particle JSON が見つかりません（ビルド後の Assets コピーを確認）");
#endif
    params.emplace_back();
  }

  InitializeFromParams(params);
}

void Emitter::RebuildFromParams(const std::vector<Param> &params) {
  InitializeFromParams(params);
  RestartPreview();
}

std::vector<Param> Emitter::ExportParams() const {
  std::vector<Param> params;
  params.reserve(layers_.size());
  for (const Layer &layer : layers_) {
    params.push_back(layer.param);
  }
  return params;
}

void Emitter::RestartPreview() {
  PrepareForOneShot();
  Emit();
}

void Emitter::PrepareForOneShot() {
  for (size_t i = 0; i < layers_.size(); ++i) {
    Layer &layer = layers_[i];
    layer.accumulator = 0.0f;
    layer.elapsedSeconds = 0.0f;
    layer.hasBurst = false;
    layer.isFinished = false;

    for (Particle &particle : particles_[i]) {
      particle.isActive = false;
    }
  }
}

void Emitter::Emit() {

  for (size_t i = 0; i < layers_.size(); ++i) {

    if (layers_[i].param.emitMode == EmitMode::CONTINUOUS)
      continue;

    EmitOneLayer(i);
  }
}

void Emitter::Update() {

  /* レイヤー管理 */

  bool isLayerAnyParticleActive = false;

  for (size_t i = 0; i < layers_.size(); ++i) {
    auto &layer = layers_[i];

    if (layer.isFinished)
      continue;

    auto &layerParticles = particles_[i];

    ResolveContinuousEmit(i);

    isLayerAnyParticleActive = false;

    for (auto &p : layerParticles) {
      if (!p.isActive)
        continue;

      p.lifeTimePassed += frameDeltaTime_->GetDeltaTime();

      if (p.lifeTimePassed >= p.lifeDuration) {
        p.isActive = false;
        continue;
      }

      isLayerAnyParticleActive = true;

      p.velocity += p.acceleration * frameDeltaTime_->GetDeltaTime();
      p.position += p.velocity * frameDeltaTime_->GetDeltaTime();

      p.currentScale = Math::Lerp(p.startScale, p.endScale,
                                  p.lifeTimePassed / p.lifeDuration);

      p.currentColor = Math::Lerp(p.startColor, p.endColor,
                                  p.lifeTimePassed / p.lifeDuration);
    }

    layer.elapsedSeconds += frameDeltaTime_->GetDeltaTime();

    if (layer.param.emitMode == EmitMode::BURST) {
      if (!isLayerAnyParticleActive && layer.hasBurst) {
        layer.isFinished = true;
      }
    } else if (layer.param.emitMode == EmitMode::CONTINUOUS) {
      if (!isLayerAnyParticleActive &&
          layer.elapsedSeconds >= layer.param.duration) {
        layer.isFinished = true;
      }
    }
  }
}

size_t Emitter::ResolveSpawnCount(Layer &layer) {

  if (layer.param.emitMode == EmitMode::BURST) {
    return layer.param.burstCount;

  } else if (layer.param.emitMode == EmitMode::CONTINUOUS) {
    return 1;
  }

  return 0;
}

Math::Vector3 Emitter::ResolveSpawnLocalPos(Layer &layer) {

  switch (layer.param.emitShape) {

  case EmitShape::AABB:
    return Geometry::Random::InAABB(layer.param.emitAabb);

  case EmitShape::CIRCLE_XZ:

    return Geometry::Random::InCircleXZ(layer.param.emitCircle.radius,
                                        layer.param.emitCircle.center);

  case EmitShape::CIRCLE_XY:
    return Geometry::Random::InCircleXY(layer.param.emitCircle.radius,
                                        layer.param.emitCircle.center);

  case EmitShape::CIRCLE_YZ:

    return Geometry::Random::InCircleYZ(layer.param.emitCircle.radius,
                                        layer.param.emitCircle.center);

  case EmitShape::SPHERE:
    return Geometry::Random::InSphere(layer.param.emitSphere);

  case EmitShape::POINT:
    return layer.param.emitPointPos;
  default:
    return Math::Vector3::Zero();
  }
}

Math::Vector3
Emitter::ResolveSimulationSpace(SimulationSpace &simulationSpaceFromEmitter,
                                Math::Vector3 localPos) {
  if (simulationSpaceFromEmitter == SimulationSpace::WORLD) {
    return transform_.MakeWorldMatrix().Transform(localPos);
  } else {
    return localPos;
  }
}

void Emitter::ParticleInit(Particle &particle, Layer &layer) {

  Math::Vector3 localPos = ResolveSpawnLocalPos(layer);

  particle.isActive = true;

  particle.position =
      ResolveSimulationSpace(layer.param.simulationSpaceFromEmitter, localPos);

  particle.lifeDuration = Alg::Random::Float(layer.param.minLifeDuration,
                                             layer.param.maxLifeDuration);

  Math::Vector3 randomVel{Alg::Random::Float(-layer.param.velocityRandom.x,
                                             layer.param.velocityRandom.x),
                          Alg::Random::Float(-layer.param.velocityRandom.y,
                                             layer.param.velocityRandom.y),
                          Alg::Random::Float(-layer.param.velocityRandom.z,
                                             layer.param.velocityRandom.z)};

  switch (layer.param.movementMode) {
  case MovementMode::DIRECTIONAL:

  case MovementMode::RADIAL_SPREAD: {
    Math::Vector3 dir = localPos;
    if (dir.LengthSquare() > 1e-5f) {
      dir.Normalize();
    } else {
      dir = {0.0f, 1.0f, 0.0f};
    }
    float speed = layer.param.baseVelocity.Length();
    particle.velocity = (dir * speed) + randomVel;
    break;
  }
  default:
    particle.velocity = layer.param.baseVelocity + randomVel;
    break;
  }

  particle.lifeTimePassed = 0.0f;

  particle.startColor = layer.param.startColor;
  particle.endColor = layer.param.endColor;
  particle.currentColor = particle.startColor;

  particle.startScale = layer.param.startScale;
  particle.endScale = layer.param.endScale;
  particle.currentScale = particle.startScale;
}

void Emitter::EmitOneLayer(const size_t layerIndex) {

  auto &layer = layers_[layerIndex];
  auto &layerParticles = particles_[layerIndex];

  if (layer.hasBurst)
    return;

  size_t spawnCount = ResolveSpawnCount(layer);

  size_t spawnedCount = 0;

  for (auto &p : layerParticles) {
    if (!p.isActive) {

      ParticleInit(p, layer);

      ++spawnedCount;

      if (spawnedCount >= spawnCount) {
        break;
      }
    }
  }

  if (layer.param.emitMode == EmitMode::BURST && spawnedCount > 0) {
    layer.hasBurst = true;
  }
}

void Emitter::ResolveContinuousEmit(const size_t layerIndex) {

  auto &layer = layers_[layerIndex];

  if (layer.elapsedSeconds >= layer.param.duration)
    return;

  if (layer.param.emitMode == EmitMode::CONTINUOUS) {

    layer.accumulator += frameDeltaTime_->GetDeltaTime();

    while (layer.accumulator >= 1.0f / layer.param.emitRate) {
      EmitOneLayer(layerIndex);
      layer.accumulator -= 1.0f / layer.param.emitRate;
    }
  }
}

void Emitter::Draw() {

  Atrum::Engine *engine = Atrum::Engine::Instance();
  if (engine == nullptr) {
    return;
  }

  if (camera_ != nullptr) {
    engine->SetCamera(camera_);
  }

  Math::TransformLH particleTransform{};

  for (size_t i = 0; i < layers_.size(); ++i) {

    const auto &layer = layers_[i];

    if (layer.isFinished)
      continue;

    const auto &layerParticles = particles_[i];

    for (const auto &p : layerParticles) {
      if (!p.isActive)
        continue;

      particleTransform.translate = p.position;
      particleTransform.quaternion = p.rotation;
      particleTransform.scale = p.currentScale;

      switch (layer.param.renderMode) {
      case RenderMode::BUILTIN_SPHERE: {
        const float radius =
            std::max({p.currentScale.x, p.currentScale.y, p.currentScale.z});
        // 戦闘演出は同フレーム大量描画になりやすいため细分を抑える
        constexpr uint32_t kBuiltinSphereSubdivision = 2;
        engine->DrawSphere(WhiteTextureIndex(), p.currentColor, Math::Transform{},
                           particleTransform.MakeWorldMatrix(), radius,
                           kBuiltinSphereSubdivision, false);
        break;
      }
      case RenderMode::MODEL:
        if (layer.model != nullptr) {
          engine->DrawModel(layer.model.get(),
                            particleTransform.MakeWorldMatrix(), false,
                            p.currentColor);
        }
        break;
      case RenderMode::BILLBOARD: {
        Math::Matrix4x4 billboardWorld = Math::Matrix4x4::Identity();
        if (camera_ != nullptr) {
          billboardWorld = MakeViewPlaneParallelBillboardWorld(
              p.position, p.currentScale, camera_->GetViewMatrix());
        }
        engine->DrawBillboard(layer.billboardTextureIndex, p.currentColor,
                              Math::Transform{}, billboardWorld, false);
        break;
      }
      default:
        break;
      }
    }
  }
}

} // namespace Atrum::Visual::Particle