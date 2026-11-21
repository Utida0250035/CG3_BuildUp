#include "System/Editor/Entity/EntityEditorAddEmptyInstanceFromSelection.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

namespace Atrum::EntityEditor::EntityEditorAddEmptyInstanceFromSelection {

void AppendInstance(EntityEditor &editor) {
  EditorSceneDocument &scene = editor.SceneDocument();
  const std::string name =
      "Entity_" + std::to_string(scene.instances.size());

  SceneEntityInstance instance{};
  instance.instanceName = name;
  instance.parentIndex = -1;

  if (scene.instances.empty()) {
    instance.definition = MakeDefaultEntityDefinition(name);
  } else {
    editor.ClampSelectedInstanceIndex();
    instance.definition = editor.SelectedDefinition();
    instance.definition.entityName = name;
  }

  scene.instances.push_back(std::move(instance));
  UiState &ui = editor.Ui();
  ui.selectedInstanceIndex = static_cast<int>(scene.instances.size()) - 1;
  ui.selectedInstanceIndices = {ui.selectedInstanceIndex};
  editor.ClampSelectedInstanceIndex();
  EntityEditorAuthoringDocumentEditNotify::Notify(editor);
}

} // namespace Atrum::EntityEditor::EntityEditorAddEmptyInstanceFromSelection