#include "Scene/SceneEntityEditorSceneViewVisualEffectsApply.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"

namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply {

void ApplyBeforeDraw(Atrum::Engine *engine) {
  if (engine == nullptr) {
    return;
  }
  engine->FlushDirectionalLightConstantBuffer();
}

} // namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply