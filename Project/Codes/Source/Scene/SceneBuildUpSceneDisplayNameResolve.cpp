#include "Scene/SceneBuildUpSceneDisplayNameResolve.h"

#include "System/Editor/Scene/EditorSceneAsset.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::SceneBuildUpSceneDisplayNameResolve {

std::string ResolveInitialSceneDisplayName(const std::string &jsonFilePath) {
  const std::string pathFallback =
      Editor::Scene::SceneDisplayNameFromPath(jsonFilePath);
  if (jsonFilePath.empty()) {
    return pathFallback;
  }

  namespace fs = std::filesystem;
  if (!fs::exists(jsonFilePath)) {
    return pathFallback;
  }

  std::ifstream file(jsonFilePath);
  if (!file.is_open()) {
    return pathFallback;
  }

  nlohmann::json json{};
  try {
    file >> json;
  } catch (const nlohmann::json::exception &) {
    return pathFallback;
  }

  if (json.contains("sceneName") && json["sceneName"].is_string()) {
    const std::string sceneName = json["sceneName"].get<std::string>();
    if (!sceneName.empty()) {
      return sceneName;
    }
  }

  return pathFallback;
}

} // namespace Atrum::SceneBuildUpSceneDisplayNameResolve