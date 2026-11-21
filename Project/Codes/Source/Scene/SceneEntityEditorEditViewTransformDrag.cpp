#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorEditViewTransformDrag.h"

#include "Camera/Camera.h"
#include "ForDebug/ImGui.h"
#include "Scene/SceneEntityEditor.h"
#include "System/Editor/EditorEditViewScreenRay.h"
#include "System/Editor/EditorFrame.h"
#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::SceneEntityEditorEditViewTransformDrag {

namespace {

constexpr float kPlacementPlaneY = 0.05f;

struct DragState {
  bool active = false;
  Math::Vector3 grabOffset{};
};

DragState &State() {
  static DragState state;
  return state;
}

EntityEditor::EntityEditor *ResolveEditor(SceneEntityEditor &editor) {
  return editor.MutableEntityEditor();
}

} // namespace

void Tick(SceneEntityEditor &editor) {
  EntityEditor::EntityEditor *entityEditor = ResolveEditor(editor);
  if (entityEditor == nullptr) {
    return;
  }

  EntityEditor::UiState &ui = entityEditor->Ui();
  if (ui.interactionMode != EntityEditor::EditorInteractionMode::Edit ||
      ui.previewSimulationEnabled) {
    State().active = false;
    return;
  }

  Editor::EditorFrame &frame = Editor::EditorFrame::Instance();
  if (!frame.EditViewHovered()) {
    State().active = false;
    return;
  }

  if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Right]) {
    State().active = false;
    return;
  }

  Camera *camera = editor.GetCamera();
  if (camera == nullptr) {
    return;
  }

  float localX = 0.0f;
  float localY = 0.0f;
  if (!frame.TryMapMouseToEditViewLocal(localX, localY)) {
    State().active = false;
    return;
  }

  const Editor::SceneViewDisplaySize display =
      frame.SceneViewDisplaySizeOf(Editor::SceneViewTab::Edit);
  if (display.width < 2.0f || display.height < 2.0f) {
    return;
  }

  Math::Vector3 hit{};
  if (!Editor::TryPickHorizontalPlaneY(*camera, localX, localY, display.width,
                                     display.height, kPlacementPlaneY, hit)) {
    State().active = false;
    return;
  }

  EntityEditor::EditorSceneDocument &document = entityEditor->SceneDocument();
  if (document.instances.empty()) {
    return;
  }
  entityEditor->ClampSelectedInstanceIndex();
  const size_t index =
      static_cast<size_t>(ui.selectedInstanceIndex);
  if (index >= document.instances.size()) {
    return;
  }

  EntityEditor::SceneEntityInstance &instance =
      document.instances[index];
  Math::Vector3 &translate = instance.definition.initialTransform.translate;

  DragState &drag = State();

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      !ImGui::GetIO().KeyCtrl) {
    drag.active = true;
    drag.grabOffset = translate - hit;
    ui.importStatusMessage =
        "エディットビュー: 左ドラッグで配置（右ボタンでカメラ操作）";
  }

  if (!drag.active || !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    drag.active = false;
    return;
  }

  translate.x = hit.x + drag.grabOffset.x;
  translate.z = hit.z + drag.grabOffset.z;
  entityEditor->MarkSceneDocumentDirty();
  ui.requestApplyPreview = true;
}

} // namespace Atrum::SceneEntityEditorEditViewTransformDrag

#else

namespace Atrum::SceneEntityEditorEditViewTransformDrag {

void Tick(SceneEntityEditor &) {}

} // namespace Atrum::SceneEntityEditorEditViewTransformDrag

#endif