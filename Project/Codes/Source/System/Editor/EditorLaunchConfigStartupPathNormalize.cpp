#include "System/Editor/EditorLaunchConfigStartupPathNormalize.h"

#include <filesystem>

namespace Atrum::Editor::LaunchConfigStartupPathNormalize {

std::string NormalizeForLaunchConfig(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty()) {
    return {};
  }

  namespace fs = std::filesystem;
  std::error_code ec{};
  fs::path normalized = fs::path(sceneJsonPath).lexically_normal();

  if (!normalized.is_absolute()) {
    return normalized.generic_string();
  }

  const fs::path cwd = fs::current_path(ec);
  if (ec) {
    return normalized.generic_string();
  }

  const fs::path relative = fs::relative(normalized, cwd, ec);
  if (!ec && !relative.empty()) {
    const std::string relativeGeneric = relative.generic_string();
    if (relativeGeneric.rfind("..", 0) != 0 &&
        relativeGeneric.find("/../") == std::string::npos) {
      return relativeGeneric;
    }
  }

  return normalized.generic_string();
}

} // namespace Atrum::Editor::LaunchConfigStartupPathNormalize