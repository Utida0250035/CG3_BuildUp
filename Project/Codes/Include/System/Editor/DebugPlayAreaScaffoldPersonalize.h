#pragma once

#include <nlohmann/json_fwd.hpp>
#include <string>

namespace Atrum::Editor {

std::string SanitizeDebugPlayAreaFileStem(const std::string &sceneName);

/// sceneName 以外に instanceName／entityName／tagCatalog をシーン名に合わせる。
void PersonalizeDebugPlayAreaSceneJson(nlohmann::json &data,
                                       const std::string &sceneName);

} // namespace Atrum::Editor