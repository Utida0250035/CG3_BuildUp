#include "System/Editor/Entity/EntityEditorRestoreSelectionAfterSceneLoad.h"

#include "System/Editor/Entity/EntityEditor.h"

#include <algorithm>

namespace Atrum::EntityEditor::EntityEditorRestoreSelectionAfterSceneLoad {

SelectionSnapshot Capture(const EntityEditor &editor) {
  const EditorSceneDocument &scene = editor.SceneDocument();
  const UiState &ui = editor.Ui();
  SelectionSnapshot snapshot{};
  snapshot.selectedComponentIndex = ui.selectedComponentIndex;
  snapshot.selectedComponentKind = ui.selectedComponentKind;

  std::vector<int> indices = ui.selectedInstanceIndices;
  if (indices.empty() && ui.selectedInstanceIndex >= 0) {
    indices.push_back(ui.selectedInstanceIndex);
  }
  std::sort(indices.begin(), indices.end());
  indices.erase(std::unique(indices.begin(), indices.end()), indices.end());

  for (const int index : indices) {
    if (index < 0 || index >= static_cast<int>(scene.instances.size())) {
      continue;
    }
    const std::string &name =
        scene.instances[static_cast<size_t>(index)].instanceName;
    if (name.empty()) {
      continue;
    }
    snapshot.selectedInstanceNames.push_back(name);
  }
  return snapshot;
}

void Apply(EntityEditor &editor, const SelectionSnapshot &snapshot) {
  EditorSceneDocument &scene = editor.SceneDocument();
  UiState &ui = editor.Ui();

  std::vector<int> restored;
  restored.reserve(snapshot.selectedInstanceNames.size());
  for (const std::string &name : snapshot.selectedInstanceNames) {
    for (int index = 0; index < static_cast<int>(scene.instances.size());
         ++index) {
      if (scene.instances[static_cast<size_t>(index)].instanceName == name) {
        restored.push_back(index);
        break;
      }
    }
  }

  if (restored.empty() && !scene.instances.empty()) {
    restored.push_back(0);
  }

  ui.selectedInstanceIndices = restored;
  ui.selectedInstanceIndex = restored.empty() ? 0 : restored.front();
  editor.ClampSelectedInstanceIndex();

  if (scene.instances.empty()) {
    ui.selectedComponentIndex = 0;
    ui.selectedComponentKind = snapshot.selectedComponentKind;
    return;
  }

  const EntityDefinition &definition = editor.SelectedDefinition();
  if (definition.components.empty()) {
    ui.selectedComponentIndex = 0;
    ui.selectedComponentKind = snapshot.selectedComponentKind;
    return;
  }

  const int maxIndex =
      static_cast<int>(definition.components.size()) - 1;
  ui.selectedComponentIndex =
      std::clamp(snapshot.selectedComponentIndex, 0, maxIndex);
  ui.selectedComponentKind =
      definition.components[static_cast<size_t>(ui.selectedComponentIndex)]
          .kind;
}

} // namespace Atrum::EntityEditor::EntityEditorRestoreSelectionAfterSceneLoad