#include "Scene/SceneBuildUpRuntimeViewStatusText.h"

#include "Scene/SceneBuildUp.h"

namespace Atrum {

std::string BuildSceneBuildUpRuntimeViewStatusLine(const SceneBuildUp &scene) {
  std::string line = "機能検証 BuildUp";
  if (!scene.SceneDisplayName().empty()) {
    line += " — ";
    line += scene.SceneDisplayName();
  }
  if (!scene.SceneJsonPath().empty()) {
    line += " | ";
    line += scene.SceneJsonPath();
  }
  if (scene.SceneDocumentLoadFailed()) {
    line += " | 読込失敗";
  }
  return line;
}

} // namespace Atrum