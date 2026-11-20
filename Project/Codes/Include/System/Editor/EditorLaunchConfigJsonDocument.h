#pragma once

#include "System/Editor/Scene/EditorSceneAsset.h"
#include <nlohmann/json.hpp>

namespace Atrum::Editor::LaunchConfigJsonDocument {

nlohmann::json BuildDocument(const Scene::LaunchConfig &config);

bool TryParseStartupScenePath(const nlohmann::json &json, std::string &outPath);

} // namespace Atrum::Editor::LaunchConfigJsonDocument