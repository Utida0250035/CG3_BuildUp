#pragma once

#include "System/Editor/EditorFrame.h"

#include <memory>
#include <vector>

namespace Atrum {

class Entity;
class SceneEntityEditor;

/// Runtime / Game タブ描画直前のカメラ適用（Runtime は CmpCamera 優先、Game は StageView）。
namespace SceneEntityEditorRuntimeViewCameraDraw {

void PrepareSceneViewCameraForDraw(
    SceneEntityEditor &editor, Editor::SceneViewTab viewTab,
    const std::vector<std::unique_ptr<Entity>> &previewEntities,
    bool runtimeFunctionalViewDraw);

} // namespace SceneEntityEditorRuntimeViewCameraDraw

} // namespace Atrum