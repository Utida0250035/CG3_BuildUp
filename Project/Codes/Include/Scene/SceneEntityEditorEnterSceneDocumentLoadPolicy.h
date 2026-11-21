#pragma once

#include <string>

namespace Atrum {

namespace EntityEditor {
class EntityEditor;
}

/// EnterScene でディスク読込が必要か（再入場時はメモリ上の編集を維持）。
struct SceneEntityEditorEnterSceneLoadPlan {
  bool loadFromDisk = false;
  std::string jsonPathToLoad;
};

SceneEntityEditorEnterSceneLoadPlan PlanEnterSceneDocumentLoad(
    const std::string &pendingBootJsonPath, EntityEditor::EntityEditor &editor,
    bool hasEnteredSceneEntityEditorBefore);

} // namespace Atrum