#pragma once

#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "System/Hash64.h"
#include "System/Particle/Particle.h"
#include "System/Particle/ParticleParam.h"
#include <memory>
#include <random>
#include <vector>

namespace Atrum {
class FrameDeltaTime;
class AssetModel;
class Camera;
} // namespace Atrum

namespace Atrum::Visual::Particle {

class Emitter {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;

  struct Layer {
    Param param;

    // CONTINUOUS型 断続発射用の蓄積タイマー
    float accumulator = 0.0f;
    // CONTINUOUS型 レイヤー発生からの経過時間(パーティクルの寿命とは別)
    float elapsedSeconds = 0.0f;
    // BURST型 バースト済みフラグ
    bool hasBurst = false;
    // 共通 終了フラグ
    bool isFinished = false;
    // 共通 使用3Dモデル
    std::shared_ptr<AssetModel> model = nullptr;
  };

#ifdef DEVELOPMENT

  std::string name_ = "";

#endif

  Camera *camera_ = nullptr;
  uint64_t nameHash_ = 0;
  std::vector<Layer> layers_{};
  std::vector<std::vector<Particle>> particles_{};
  Math::TransformLH transform_{};
  bool isAlive_ = false;
  bool isEmitting_ = false;

  size_t ResolveSpawnCount(Layer &layer);
  Math::Vector3 ResolveSpawnLocalPos(Layer &layer);
  Math::Vector3
  ResolveSimulationSpace(SimulationSpace &simulationSpaceFromEmitter,
                         Math::Vector3 localPos);

  void ParticleInit(Particle &particle, Layer &layer);

  void EmitOneLayer(const size_t layerIndex);

  void ResolveContinuousEmit(const size_t layerIndex);

public:
  Emitter(const char *emitterName, const Math::TransformLH &emitterTransform)
      : nameHash_(Hash64(emitterName)), isAlive_(true),
        transform_(emitterTransform) {

#ifdef DEVELOPMENT
    name_ = emitterName;
#endif
  }

  bool IsAlive() const { return isAlive_; }
  const std::vector<std::vector<Particle>> &GetParticles() const {
    return particles_;
  }

  void Initialize(const char *jsonFilePath);
  void Emit();
  void Update();
  void Draw();

#ifdef DEVELOPMENT
  const std::string &GetName() const { return name_; }
#endif

  const Math::TransformLH &GetTransform() const { return transform_; }

  void SetTransform(const Math::TransformLH &transform) {
    transform_ = transform;
  }

  void SetCamera(Camera *camera) { camera_ = camera; }
};

} // namespace Atrum::Visual::Particle