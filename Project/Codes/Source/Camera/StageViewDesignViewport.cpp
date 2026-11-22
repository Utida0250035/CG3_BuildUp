#include "Camera/StageViewDesignViewport.h"

#include "Engine/EngineAccess.h"
#include "Engine/Platform/Window/Window.h"
#if defined(DEVELOPMENT)
#include "System/Editor/Camera/StageViewCameraTuning.h"
#endif

namespace Atrum {

float GetStageViewDesignAspectRatio() {
#if defined(DEVELOPMENT)
  Editor::StageViewCameraTuning &tuning =
      Editor::StageViewCameraTuning::Instance();
  if (tuning.IsGameViewPreviewEnabled()) {
    tuning.EnsureDesignBaselineLoaded();
    if (tuning.IsLoaded() && tuning.DesignClientHeight() > 0) {
      return static_cast<float>(tuning.DesignClientWidth()) /
             static_cast<float>(tuning.DesignClientHeight());
    }
  }
#endif

  int32_t clientWidth = 0;
  int32_t clientHeight = 0;
  if (EngineTryGetClientViewportSize(clientWidth, clientHeight) &&
      clientHeight > 0) {
    return static_cast<float>(clientWidth) /
           static_cast<float>(clientHeight);
  }

#if defined(DEVELOPMENT)
  Editor::StageViewCameraTuning &tuningFallback =
      Editor::StageViewCameraTuning::Instance();
  tuningFallback.EnsureDesignBaselineLoaded();
  if (tuningFallback.IsLoaded() && tuningFallback.DesignClientHeight() > 0) {
    return static_cast<float>(tuningFallback.DesignClientWidth()) /
           static_cast<float>(tuningFallback.DesignClientHeight());
  }
#endif

  return static_cast<float>(Window::kDefaultClientWidth) /
         static_cast<float>(Window::kDefaultClientHeight);
}

} // namespace Atrum