#include "Scene/SceneParticleEditorAllowDebugPolicy.h"

namespace Atrum {

bool AllowDebugCameraControlInParticleEditorScene() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  // ステージロック無しのプレビュー専用。GameOrder 経路で DebugCamera を操作する。
  return true;
#else
  return false;
#endif
}

} // namespace Atrum