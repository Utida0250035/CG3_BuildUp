#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorSceneRuntimeFunctionalViewDraw.h"

#include "Scene/SceneEntityEditorSceneViewVisualEffectsApply.h"
#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Blend/BlendMode.h"
#include "Scene/Scene.h"
#include "Scene/SceneBuildUpPlayerSilhouetteDraw.h"
#include "Scene/SceneBuildUpStageCameraFraming.h"
#include "System/Editor/EditorFrame.h"
#include "Camera/Camera.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Particle/EmitterManager.h"

namespace Atrum::Editor {

namespace {

void DrawRootEntities(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  for (const std::unique_ptr<Entity> &entity : entities) {
    if (!entity) {
      continue;
    }
    if (entity->GetParent() == nullptr) {
      entity->Draw();
    }
  }
}

} // namespace

void DrawEditorSceneRuntimeFunctionalView(
    Scene &scene, const EntityEditor::EditorSceneDocument &/*document*/,
    const std::vector<std::unique_ptr<Entity>> &entities, const int width,
    const int height) {
  if (width <= 0 || height <= 0) {
    return;
  }

  AtrumEngine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);

  SceneBuildUpStageCamera::MaintainLockedStageCamera(scene);

  Atrum::Camera *camera = scene.GetCamera();
  if (camera == nullptr) {
    return;
  }

  FitCameraPerspectiveToViewport(camera, width, height);
  engine->SetCamera(camera);
  SceneEntityEditorSceneViewVisualEffectsApply::ApplyBeforeDraw(engine);

  DrawRootEntities(entities);
  Atrum::Particle::DrawEmitterManagerForActiveScene();
  SceneBuildUpPlayerSilhouette::DrawAfterRuntimeEntities(engine, entities);
}

} // namespace Atrum::Editor

#endif