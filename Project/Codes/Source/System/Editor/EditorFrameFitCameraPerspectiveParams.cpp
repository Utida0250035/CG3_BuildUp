#include "System/Editor/EditorFrameFitCameraPerspectiveParams.h"

#include "System/Editor/Camera/StageViewCameraTuning.h"

namespace Atrum::Editor::EditorFrameFitCameraPerspectiveParams {

float ResolveViewportPerspectiveFovY() {
  StageViewCameraTuning &tuning = StageViewCameraTuning::Instance();
  tuning.EnsureDesignBaselineLoaded();
  return tuning.PerspectiveFovY();
}

} // namespace Atrum::Editor::EditorFrameFitCameraPerspectiveParams