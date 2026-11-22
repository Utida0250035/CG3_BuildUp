#include "Scene/CommandSceneDevelopmentBoot.h"

#include "Scene/CommandSceneBootLaunchPathGate.h"
#include "System/Editor/EditorDebugPlayAreaFeatureVerificationCanonical.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/GameAssetsPaths.h"

#include <filesystem>

namespace Atrum {

namespace {

std::string ResolveConfiguredStartupScenePath() {
  const std::string playRequestPath =
      Editor::Scene::TakePendingEditorPlayScenePath();
  if (!playRequestPath.empty()) {
    return playRequestPath;
  }
  return Editor::Scene::ResolveStartupScenePath();
}

bool IsDebugPlayAreaScenePath(const std::string &sceneJsonPath) {
  return sceneJsonPath.find(EditorAssets::kDebugPlayAreaDirectory) !=
             std::string::npos ||
         sceneJsonPath.find("DebugPlayArea") != std::string::npos;
}

bool IsRuntimePlayScenePath(const std::string &sceneJsonPath) {
  return sceneJsonPath.rfind(GameAssets::kScenesDirectory, 0) == 0 ||
         sceneJsonPath.find("/Assets/Scenes/") != std::string::npos ||
         sceneJsonPath.find("\\Assets\\Scenes\\") != std::string::npos;
}

} // namespace

bool IsValidBootSceneJsonPath(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty()) {
    return false;
  }
  namespace fs = std::filesystem;
  if (!fs::exists(sceneJsonPath)) {
    return false;
  }
  EntityEditor::EditorSceneDocument document{};
  return EntityEditor::TryLoadSceneDocument(sceneJsonPath, document);
}

DevelopmentBootTarget ResolveDevelopmentBootTarget() {
  DevelopmentBootTarget target{};
  std::string sceneJsonPath = ResolveConfiguredStartupScenePath();
  const char *const featureVerificationFallback =
      DebugPlayAreaFeatureVerificationCanonical::SceneJsonPath();
  if (sceneJsonPath.empty()) {
    if (CanLaunchBootSceneAtPath(featureVerificationFallback)) {
      target.kind = DevelopmentBootKind::SceneBuildUp;
      target.sceneJsonPath = featureVerificationFallback;
    }
    return target;
  }
  if (!CanLaunchBootSceneAtPath(sceneJsonPath)) {
    if (CanLaunchBootSceneAtPath(featureVerificationFallback)) {
      target.kind = DevelopmentBootKind::SceneBuildUp;
      target.sceneJsonPath = featureVerificationFallback;
    }
    return target;
  }

  target.sceneJsonPath = sceneJsonPath;
  if (IsDebugPlayAreaScenePath(sceneJsonPath)) {
    target.kind = DevelopmentBootKind::SceneBuildUp;
  } else if (IsRuntimePlayScenePath(sceneJsonPath)) {
    target.kind = DevelopmentBootKind::SceneBuildUpPlayRuntime;
  } else {
    target.kind = DevelopmentBootKind::EntityEditorWithJson;
  }
  return target;
}

} // namespace Atrum