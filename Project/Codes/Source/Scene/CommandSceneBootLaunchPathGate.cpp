#include "Scene/CommandSceneBootLaunchPathGate.h"

#include <filesystem>

namespace Atrum {

bool CanLaunchBootSceneAtPath(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty()) {
    return false;
  }
  namespace fs = std::filesystem;
  return fs::exists(sceneJsonPath);
}

} // namespace Atrum