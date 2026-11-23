#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneBuildUpDedicatedRuntimeViewWindow.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneBuildUpRuntimeViewStatusText.h"
#include "System/Editor/EditorFrame.h"
#include "System/Editor/EditorSceneRenderTarget.h"
#include "System/Editor/EditorSceneViewPanel.h"

namespace Atrum::SceneBuildUpDedicatedRuntimeViewWindow {

void LayoutView(SceneBuildUp &scene) {
  static std::string statusLineStorage;
  statusLineStorage = BuildSceneBuildUpRuntimeViewStatusLine(scene);

  LayoutDedicatedSceneViewWindow(
      kWindowTitle, Editor::SceneViewTab::Runtime, statusLineStorage.c_str(),
      "F1: 戻る");
}

void RenderViews(SceneBuildUp &scene) {
  Atrum::Engine *const engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  Editor::EditorSceneRenderTargetHub::Instance().RenderSceneViews(
      engine, [&scene](const Editor::SceneViewTab tab, const int width,
                       const int height) {
        if (tab != Editor::SceneViewTab::Runtime) {
          return;
        }
        scene.DrawRuntimeViewContent(width, height);
      });
}

void LayoutAndRenderViews(SceneBuildUp &scene) {
  LayoutView(scene);
  RenderViews(scene);
}

} // namespace Atrum::SceneBuildUpDedicatedRuntimeViewWindow

#endif