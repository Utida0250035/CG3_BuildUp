#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Entity/EntityEditorReleaseStartupBuildUp.h"

#include "Scene/CommandSceneDevelopmentBoot.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum::EntityEditor {

bool TryQueueBuildUpFromReleaseStartup(UiState &ui,
                                       std::string *statusMessage) {
  const std::string startupPath =
      Editor::Scene::ResolveStartupScenePath();
  if (startupPath.empty()) {
    if (statusMessage != nullptr) {
      *statusMessage =
          "Release 起動シーンが未設定です（「起動シーンに設定」を先に行ってください）";
    }
    return false;
  }
  if (!IsValidBootSceneJsonPath(startupPath)) {
    if (statusMessage != nullptr) {
      *statusMessage =
          "Release 起動 JSON を読み込めません: " + startupPath;
    }
    return false;
  }

  ui.sceneBuildUpLaunchPath = startupPath;
  ui.requestLaunchSceneBuildUp = true;
  if (statusMessage != nullptr) {
    *statusMessage = "Release 起動シーンで BuildUp を予約しました";
  }
  return true;
}

} // namespace Atrum::EntityEditor

#endif