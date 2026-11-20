#include "Scene/SceneEntityEditorDebugHotkeys.h"

#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/Scene.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#include "Scene/SceneParticleEditor.h"
#include "System/Editor/EditorDebugPlayAreaCombatBoundsSandbox.h"
#include "System/Editor/EditorDebugPlayAreaFeatureVerificationCanonical.h"
#include "System/GameAssetsPaths.h"
#include "Engine/EngineAccess.h"

namespace Atrum::SceneEntityEditorDebugHotkeys {

void Tick(Scene *returnScene) {
  if (returnScene == nullptr) {
    return;
  }

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F2)) {
    SceneBuildUpLaunchPolicy::QueueFadeToBuildUp(
        DebugPlayAreaFeatureVerificationCanonical::SceneJsonPath(), returnScene);
    return;
  }

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F4)) {
    CommandChangeScene::Instance()->Set(
        SceneParticleEditor::Instance(),
        std::make_unique<ChangeSceneFade>().release());
    return;
  }

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F3)) {
    const std::string demoScene =
        std::string(GameAssets::kScenesDirectory) + "/ParticleDemo.json";
    SceneBuildUpLaunchPolicy::QueueFadeToBuildUp(demoScene, returnScene);
    return;
  }

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F6)) {
    SceneBuildUpLaunchPolicy::QueueFadeToBuildUp(
        DebugPlayAreaCombatBoundsSandbox::SceneJsonPath(), returnScene);
  }
}

} // namespace Atrum::SceneEntityEditorDebugHotkeys