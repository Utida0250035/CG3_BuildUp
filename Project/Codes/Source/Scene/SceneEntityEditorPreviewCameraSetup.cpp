#include "Scene/SceneEntityEditorPreviewCameraSetup.h"

#include "Scene/ScenePrimaryCameraResolve.h"
#include "System/Entity/Components/CmpCamera.h"
#include "System/Entity/Entity.h"

#include <memory>
#include <vector>

namespace Atrum::SceneEntityEditorPreviewCameraSetup {

bool ApplyPrimaryFromPreviewEntities(
    const std::vector<std::unique_ptr<Entity>> &previewEntities,
    const bool enableDebugCameraInput) {
  if (previewEntities.empty()) {
    return false;
  }
  static const std::vector<std::unique_ptr<Entity>> kEmptyRigEntities{};
  CmpCamera *cmp =
      ResolvePrimaryCmpCamera(kEmptyRigEntities, &previewEntities);
  if (cmp == nullptr) {
    return false;
  }
  cmp->PrepareBeforeDraw(enableDebugCameraInput);
  return true;
}

} // namespace Atrum::SceneEntityEditorPreviewCameraSetup