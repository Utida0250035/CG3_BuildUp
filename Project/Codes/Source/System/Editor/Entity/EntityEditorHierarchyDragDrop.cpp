#include "System/Editor/Entity/EntityEditorHierarchyDragDrop.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
#include "System/Editor/Entity/EntityEditorHierarchyDuplicateAsChild.h"
#include "System/Editor/Entity/EntityEditorHierarchyInstanceReorder.h"
#include "System/Editor/Entity/EntityEditorHierarchyReparent.h"

#include <imgui.h>

namespace Atrum::EntityEditor::EntityEditorHierarchyDragDrop {

namespace {

constexpr const char *kHierarchyInstancePayload = "EE_HIERARCHY_INST_IDX";

void FocusSelection(EntityEditor &editor, const int index) {
  UiState &ui = editor.Ui();
  ui.selectedInstanceIndex = index;
  ui.selectedInstanceIndices = {index};
  editor.ClampSelectedInstanceIndex();
}

bool AcceptDroppedInstance(EntityEditor &editor, const int targetIndex,
                           const int sourceIndex) {
  if (sourceIndex == targetIndex) {
    return false;
  }
  bool changed = false;
  const ImGuiIO &io = ImGui::GetIO();
  if (io.KeyCtrl) {
    changed = EntityEditorHierarchyDuplicateAsChild::DuplicateUnderParent(
        editor, sourceIndex, targetIndex);
    return changed;
  }
  if (io.KeyShift) {
    int newIndex = sourceIndex;
    changed = EntityEditorHierarchyInstanceReorder::MoveAfterSibling(
        editor.SceneDocument(), sourceIndex, targetIndex, &newIndex);
    if (changed) {
      FocusSelection(editor, newIndex);
      EntityEditorAuthoringDocumentEditNotify::Notify(editor);
    }
    return changed;
  }
  changed = EntityEditorHierarchyReparent::Reparent(editor.SceneDocument(),
                                                    sourceIndex, targetIndex);
  if (changed) {
    FocusSelection(editor, sourceIndex);
    EntityEditorAuthoringDocumentEditNotify::Notify(editor);
  }
  return changed;
}

} // namespace

void AttachToHierarchyNode(EntityEditor &editor, const int nodeIndex) {
  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload(kHierarchyInstancePayload, &nodeIndex,
                              sizeof(int));
    ImGui::TextUnformatted("ドラッグ: 親変更 / Shift 並べ替え / Ctrl 複製");
    ImGui::EndDragDropSource();
  }

  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload *payload =
            ImGui::AcceptDragDropPayload(kHierarchyInstancePayload)) {
      const int sourceIndex = *static_cast<const int *>(payload->Data);
      AcceptDroppedInstance(editor, nodeIndex, sourceIndex);
    }
    ImGui::EndDragDropTarget();
  }
}

void AttachRootDropTarget(EntityEditor &editor) {
  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload *payload =
            ImGui::AcceptDragDropPayload(kHierarchyInstancePayload)) {
      const int sourceIndex = *static_cast<const int *>(payload->Data);
      const ImGuiIO &io = ImGui::GetIO();
      if (io.KeyCtrl) {
        if (EntityEditorHierarchyDuplicateAsChild::DuplicateUnderParent(
                editor, sourceIndex, -1)) {
          // 複製側で選択更新済み
        }
      } else if (io.KeyShift) {
        // ルート直下の末尾へ並べ替え
        EditorSceneDocument &scene = editor.SceneDocument();
        int lastRoot = -1;
        for (int index = 0; index < static_cast<int>(scene.instances.size());
             ++index) {
          if (scene.instances[static_cast<size_t>(index)].parentIndex < 0) {
            lastRoot = index;
          }
        }
        if (lastRoot >= 0 && lastRoot != sourceIndex) {
          int newIndex = sourceIndex;
          if (EntityEditorHierarchyInstanceReorder::MoveAfterSibling(
                  scene, sourceIndex, lastRoot, &newIndex)) {
            FocusSelection(editor, newIndex);
            EntityEditorAuthoringDocumentEditNotify::Notify(editor);
          }
        }
      } else if (EntityEditorHierarchyReparent::Reparent(editor.SceneDocument(),
                                                         sourceIndex, -1)) {
        FocusSelection(editor, sourceIndex);
        EntityEditorAuthoringDocumentEditNotify::Notify(editor);
      }
    }
    ImGui::EndDragDropTarget();
  }
}

} // namespace Atrum::EntityEditor::EntityEditorHierarchyDragDrop