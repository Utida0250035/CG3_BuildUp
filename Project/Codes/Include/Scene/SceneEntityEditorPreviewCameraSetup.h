#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

/// プレビュー Entity の primary CmpCamera を行列まで反映する。
namespace SceneEntityEditorPreviewCameraSetup {

bool ApplyPrimaryFromPreviewEntities(
    const std::vector<std::unique_ptr<Entity>> &previewEntities,
    bool enableDebugCameraInput);

} // namespace SceneEntityEditorPreviewCameraSetup

} // namespace Atrum