#include "Scene/SceneGameOperatorGuidanceBoot.h"

#include "System/GameAssetsPaths.h"
#include "UX/OperatorGuidanceSystem.h"

namespace Atrum {

bool BootSceneGameOperatorGuidance() {
  OperatorGuidance::System &guidance = OperatorGuidance::System::Instance();
  if (!guidance.LoadFromJsonFile(GameAssets::kOperatorGuidanceJson)) {
    guidance.ResetSession();
    return false;
  }
  guidance.ResetSession();
  return true;
}

} // namespace Atrum