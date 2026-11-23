#include "Scene/CommandSceneBootLaunchPathGate.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"

#include <filesystem>

namespace Atrum {

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

bool CanLaunchBootSceneAtPath(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty()) {
    return false;
  }
  namespace fs = std::filesystem;
  return fs::exists(sceneJsonPath);
}

} // namespace Atrum