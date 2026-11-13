#include "System/Particle/SceneParticleServices.h"

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Scene/CommandScene.h"
#include "Scene/Scene.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/Particle/EmitterManager.h"

#include <filesystem>

namespace Atrum::Particle {

namespace {

bool IsAbsoluteStylePath(const std::string &path) {
  return path.size() >= 2 && path.compare(0, 2, "./") == 0 ||
         path.size() >= 3 && path.compare(0, 3, "../") == 0 ||
         (!path.empty() && path.front() == '/');
}

} // namespace

Particle::Emitter::EmitterManager *EmitterManagerForActiveScene() {
  Scene *scene = CommandScene::Instance()->GetSceneNow();
  return scene != nullptr ? scene->GetParticleEmitterManager() : nullptr;
}

void TickEmitterManagerForActiveScene(Camera *camera) {
  Particle::Emitter::EmitterManager *manager = EmitterManagerForActiveScene();
  if (manager == nullptr) {
    return;
  }
  manager->SetCamera(camera);
  manager->Update();
  manager->RemoveInactive();
}

void DrawEmitterManagerForActiveScene() {
  Particle::Emitter::EmitterManager *manager = EmitterManagerForActiveScene();
  Atrum::Engine *engine = Atrum::EngineInstance();
  if (manager == nullptr || engine == nullptr) {
    return;
  }
  engine->SetBlendModeForFrame(Atrum::BlendMode::ADD);
  manager->Draw();
  engine->SetBlendModeForFrame(Atrum::BlendMode::NONE);
}

std::string ResolveParticleJsonPath(const std::string &path) {
  if (path.empty()) {
    return {};
  }
  if (IsAbsoluteStylePath(path)) {
    return path;
  }
  return std::string(EditorAssets::kParticlesDirectory) + "/" + path;
}

} // namespace Atrum::Particle
