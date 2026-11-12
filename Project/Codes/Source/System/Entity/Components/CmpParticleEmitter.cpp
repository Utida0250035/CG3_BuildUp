#include "System/Entity/Components/CmpParticleEmitter.h"

#include "Scene/CommandScene.h"
#include "Scene/Scene.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Particle/EmitterManager.h"
#include "System/Particle/ParticleEmitter.h"
#include "System/Particle/SceneParticleServices.h"

#include <filesystem>

namespace Atrum {

void CmpParticleEmitter::Initialize() {
  carouselIndex_ = 0;
  carouselElapsed_ = 0.0f;
  EnsureEmitterRegistered();
}

void CmpParticleEmitter::Finalize() {
  if (!emitterRegistered_) {
    return;
  }
  if (Particle::Emitter::EmitterManager *manager =
          Particle::EmitterManagerForActiveScene()) {
    manager->RemoveByName(ResolvedEmitterName());
  }
  emitterRegistered_ = false;
}

std::string CmpParticleEmitter::ResolvedEmitterName() const {
  if (!emitterName_.empty()) {
    return emitterName_;
  }
  return "ent_" + std::to_string(RefOwner().GetHash()) + emitterNameSuffix_;
}

std::string CmpParticleEmitter::ActiveJsonPath() const {
  if (!samplePaths_.empty()) {
  const size_t idx =
      static_cast<size_t>(carouselIndex_ % static_cast<int>(samplePaths_.size()));
    return Particle::ResolveParticleJsonPath(samplePaths_[idx]);
  }
  return Particle::ResolveParticleJsonPath(jsonFilePath_);
}

void CmpParticleEmitter::RebuildFromJson(const std::string &resolvedPath) {
  namespace fs = std::filesystem;
  if (resolvedPath.empty() || !fs::exists(resolvedPath)) {
    return;
  }

  Particle::Emitter::EmitterManager *manager =
      Particle::EmitterManagerForActiveScene();
  if (manager == nullptr) {
    return;
  }

  const std::string emitterName = ResolvedEmitterName();
  manager->RemoveByName(emitterName);

  auto emitter = std::make_unique<Visual::Particle::Emitter>();
  emitter->SetName(emitterName);
  if (Scene *scene = CommandScene::Instance()->GetSceneNow()) {
    emitter->SetCamera(scene->GetCamera());
  }
  emitter->Initialize(resolvedPath.c_str());
  manager->Register(emitterName, std::move(emitter));
  manager->SetRetainInactiveByName(emitterName, true);
  if (Visual::Particle::Emitter *live = manager->Find(emitterName)) {
    live->RestartPreview();
    SyncEmitterTransform();
  }
  emitterRegistered_ = true;
}

void CmpParticleEmitter::SyncEmitterTransform() {
  if (!followOwnerTransform_) {
    return;
  }
  Particle::Emitter::EmitterManager *manager =
      Particle::EmitterManagerForActiveScene();
  if (manager == nullptr) {
    return;
  }
  if (Visual::Particle::Emitter *emitter =
          manager->Find(ResolvedEmitterName())) {
    Math::TransformLH transform{};
    transform.translate = RefOwner().GetWorldPosition();
    emitter->SetTransform(transform);
    if (Scene *scene = CommandScene::Instance()->GetSceneNow()) {
      emitter->SetCamera(scene->GetCamera());
    }
  }
}

void CmpParticleEmitter::EnsureEmitterRegistered() {
  RebuildFromJson(ActiveJsonPath());
}

void CmpParticleEmitter::Update() {
  FrameDeltaTime *frameDelta = EngineFrameDeltaTime();
  const float deltaSeconds = frameDelta != nullptr ? frameDelta->GetDeltaTime() : 0.0f;

  if (autoCarousel_ && samplePaths_.size() > 1) {
    carouselElapsed_ += deltaSeconds;
    if (carouselElapsed_ >= carouselIntervalSeconds_) {
      carouselElapsed_ = 0.0f;
      carouselIndex_ =
          (carouselIndex_ + 1) % static_cast<int>(samplePaths_.size());
      RebuildFromJson(ActiveJsonPath());
    }
  }

  SyncEmitterTransform();
}

} // namespace Atrum
