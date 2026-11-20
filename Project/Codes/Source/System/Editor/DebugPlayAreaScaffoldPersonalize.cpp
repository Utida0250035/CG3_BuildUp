#include "System/Editor/DebugPlayAreaScaffoldPersonalize.h"

#include <nlohmann/json.hpp>

#include <cctype>

namespace Atrum::Editor {

std::string SanitizeDebugPlayAreaFileStem(const std::string &sceneName) {
  std::string stem;
  stem.reserve(sceneName.size());
  for (const unsigned char ch : sceneName) {
    if (std::isalnum(ch) != 0 || ch == '-' || ch == '_') {
      stem.push_back(static_cast<char>(ch));
    } else {
      stem.push_back('_');
    }
  }
  if (stem.empty()) {
    stem = "DebugPlayArea";
  }
  return stem;
}

namespace {

void RenameInstance(nlohmann::json &entity, const std::string &newName) {
  entity["instanceName"] = newName;
  if (!entity.contains("definition")) {
    entity["definition"] = nlohmann::json::object();
  }
  entity["definition"]["entityName"] = newName;
}

} // namespace

void PersonalizeDebugPlayAreaSceneJson(nlohmann::json &data,
                                       const std::string &sceneName) {
  const std::string safeStem = SanitizeDebugPlayAreaFileStem(sceneName);
  data["sceneName"] = sceneName;

  if (data.contains("tagCatalog") && data["tagCatalog"].is_array()) {
    bool found = false;
    for (const auto &entry : data["tagCatalog"]) {
      if (entry.is_string() && entry.get<std::string>() == sceneName) {
        found = true;
        break;
      }
    }
    if (!found) {
      data["tagCatalog"].push_back(sceneName);
    }
  }

  if (!data.contains("entities") || !data["entities"].is_array()) {
    return;
  }

  for (auto &entity : data["entities"]) {
    if (!entity.is_object() || !entity.contains("instanceName")) {
      continue;
    }
    const std::string instanceName = entity["instanceName"].get<std::string>();
    if (instanceName == "DebugProp") {
      RenameInstance(entity, safeStem + "_Prop");
    } else if (instanceName == "DebugFloorMarker") {
      RenameInstance(entity, safeStem + "_Floor");
    }
  }
}

} // namespace Atrum::Editor