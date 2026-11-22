#pragma once

#include <memory>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum::EntityEditor {
struct EditorSceneRuntimeBuildWarnings;
}

namespace Atrum::EntityEditor::EditorSceneRuntimeMainCameraSupplement {

/// シーン JSON に primary カメラが無いとき、本編と同契約で MainCamera を 1 体追加する。
void EnsureInEntityList(std::vector<std::unique_ptr<Entity>> &entities,
                        EditorSceneRuntimeBuildWarnings *warnings);

} // namespace Atrum::EntityEditor::EditorSceneRuntimeMainCameraSupplement