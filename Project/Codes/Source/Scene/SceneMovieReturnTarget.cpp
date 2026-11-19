#include "Scene/SceneMovieReturnTarget.h"

#include "Scene/SceneGame.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneEntityEditor.h"
#endif

namespace Atrum {

Scene *ResolveSceneMovieReturnTarget(Scene *returnScene) {
  if (returnScene != nullptr) {
    return returnScene;
  }
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  return SceneEntityEditor::Instance();
#else
  return SceneGame::Instance();
#endif
}

} // namespace Atrum