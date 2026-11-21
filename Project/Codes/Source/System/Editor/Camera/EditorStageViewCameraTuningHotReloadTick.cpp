#include "System/Editor/Camera/EditorStageViewCameraTuningHotReloadTick.h"

#include "System/Editor/Camera/StageViewCameraTuning.h"

namespace Atrum::Editor {

void TickStageViewCameraTuningHotReload() {
  StageViewCameraTuning::Instance().PollHotReload();
}

} // namespace Atrum::Editor