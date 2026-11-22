#include "System/Editor/EditorLaunchConfigJsonDocument.h"

#include "System/Editor/EditorLaunchConfigFeatureVerificationDefaults.h"
#include "System/Editor/EditorLaunchConfigSchema.h"

#include <nlohmann/json.hpp>

namespace Atrum::Editor::LaunchConfigJsonDocument {

nlohmann::json BuildDocument(const Scene::LaunchConfig &config) {
  nlohmann::json json{};
  json[LaunchConfigSchema::kSchemaVersionKey] = LaunchConfigSchema::kVersion;
  json[LaunchConfigSchema::kStartupScenePathKey] = config.startupScenePath;
  json[LaunchConfigSchema::kDefaultFeatureVerificationScenePathKey] =
      LaunchConfigFeatureVerificationDefaults::DefaultStartupScenePath();
  return json;
}

bool TryParseStartupScenePath(const nlohmann::json &json,
                              std::string &outPath) {
  outPath.clear();
  if (!json.contains(LaunchConfigSchema::kStartupScenePathKey)) {
    return false;
  }
  if (!json.at(LaunchConfigSchema::kStartupScenePathKey).is_string()) {
    return false;
  }
  outPath = json.at(LaunchConfigSchema::kStartupScenePathKey).get<std::string>();
  return true;
}

} // namespace Atrum::Editor::LaunchConfigJsonDocument