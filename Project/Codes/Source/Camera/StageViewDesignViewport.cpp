#include "Camera/StageViewDesignViewport.h"

#include "Engine/Platform/Window/Window.h"
#if defined(DEVELOPMENT)
#include "System/Editor/Camera/StageViewCameraTuning.h"
#endif

namespace Atrum {

float GetStageViewDesignAspectRatio() {
#if defined(DEVELOPMENT)
  Editor::StageViewCameraTuning &tuning =
      Editor::StageViewCameraTuning::Instance();
  tuning.EnsureDesignBaselineLoaded();
  if (tuning.IsLoaded() && tuning.DesignClientHeight() > 0) {
    return static_cast<float>(tuning.DesignClientWidth()) /
           static_cast<float>(tuning.DesignClientHeight());
  }
#endif

  return static_cast<float>(Window::kDefaultClientWidth) /
         static_cast<float>(Window::kDefaultClientHeight);
}

} // namespace Atrum