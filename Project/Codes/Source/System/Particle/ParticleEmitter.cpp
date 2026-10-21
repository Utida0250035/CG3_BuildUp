#include "System/Particle/ParticleEmitter.h"
#include "Engine/AtrumEngine.h"
#include "Geometry/GeometryRandom.h"
#include "Math/Lerp.h"
#include "Math/Vector3.h"
#include "System/DeltaTime.h"
#include "System/Json/JsonHandler.h"
#include "System/Random.h"
#include "Engine/AtrumEngine.h"
#include <cassert>
#include <filesystem>
#include <numbers>

namespace {
namespace Alg = Atrum::Algorithm;
namespace M = Atrum::Math;
namespace G = Atrum::Geometry;
namespace fs = std::filesystem;
} // namespace

namespace Atrum::Visual::Particle {

void Emitter::Initialize(const char *jsonFilePath) {
  frameDeltaTime_ = FrameDeltaTime::Instance();

  std::vector<Param> params{};

  if (fs::exists(jsonFilePath)) {

    params = Json::JsonHandler::LoadFromFile<std::vector<Param>>(jsonFilePath);
  } else {
    params.emplace_back();
  }

  layers_.resize(params.size());

  particles_.resize(layers_.size());

  for (size_t i = 0; i < layers_.size(); ++i) {
    layers_[i].param = params[i];

    layers_[i].model = Engine::Instance()->GetModel(
        params[i].objDirectoryPath, params[i].objFileName,
        params[i].mtlDirectoryPath, params[i].mtlFileName);

    assert(layers_[i].param.emitRate > 0.0f);
    assert(layers_[i].param.duration > 0.0f);
    assert(layers_[i].param.maxCount > 0);

    particles_[i].resize(layers_[i].param.maxCount);
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
    layer.hasBurst = true;
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
}

void Emitter::ResolveContinuousEmit(const size_t layerIndex) {

  auto &layer = layers_[layerIndex];

  if (layer.elapsedSeconds >= layer.param.duration)
    return;

  if (layer.param.emitMode == EmitMode::CONTINUOUS) {

    layer.accumulator += frameDeltaTime_->GetDeltaTime();

    while (layer.accumulator >= layer.param.emitRate) {
      EmitOneLayer(layerIndex);
      layer.accumulator -= layer.param.emitRate;
    }
  }
}

void Emitter::Draw() {

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

      Atrum::Engine::Instance()->DrawModel(layer.model.get(),
                                           particleTransform.MakeWorldMatrix(),
                                           false, p.currentColor);
    }
  }
}

} // namespace Atrum::Visual::Particle