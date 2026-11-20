#include "Scene/SceneBuildUpReturnTarget.h"

#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneEntityEditor.h"
#endif

namespace Atrum {

Scene *ResolveSceneBuildUpReturnTarget(Scene *returnScene) {
  if (returnScene != nullptr) {
    return returnScene;
  }
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  return SceneEntityEditor::Instance();
#else
  return SceneTitle::Instance();
#endif
}

} // namespace Atrum