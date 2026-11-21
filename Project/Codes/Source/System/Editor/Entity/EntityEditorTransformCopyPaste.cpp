#include "System/Editor/Entity/EntityEditorTransformCopyPaste.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor::EntityEditorTransformCopyPaste {

void CopyInitialTransformToClipboard(UiState &ui,
                                     const Math::TransformLH &transform) {
  ui.clipboardInitialTransform = transform;
}

bool PasteInitialTransformFromClipboard(UiState &ui,
                                        Math::TransformLH &transform) {
  if (!ui.clipboardInitialTransform.has_value()) {
    return false;
  }
  transform = *ui.clipboardInitialTransform;
  return true;
}

int ApplyClipboardTransformToInstanceIndices(
    EntityEditor &editor, const std::vector<int> &indices) {
  UiState &ui = editor.Ui();
  if (!ui.clipboardInitialTransform.has_value()) {
    return 0;
  }

  EditorSceneDocument &scene = editor.SceneDocument();
  int applied = 0;
  for (const int index : indices) {
    if (index < 0 || index >= static_cast<int>(scene.instances.size())) {
      continue;
    }
    scene.instances[static_cast<size_t>(index)].definition.initialTransform =
        *ui.clipboardInitialTransform;
    ++applied;
  }
  return applied;
}

} // namespace Atrum::EntityEditor::EntityEditorTransformCopyPaste