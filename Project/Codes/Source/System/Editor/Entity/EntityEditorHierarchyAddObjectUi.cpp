#include "System/Editor/Entity/EntityEditorHierarchyAddObjectUi.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAddEmptyInstanceFromSelection.h"
#include "System/Editor/Entity/EntityEditorSceneObjectAddPreset.h"

#include <imgui.h>

namespace Atrum::EntityEditor::EntityEditorHierarchyAddObjectUi {

void Draw(EntityEditor &editor) {
  const bool hasInstances = !editor.SceneDocument().instances.empty();

  static int presetListIndex = 0;
  if (!hasInstances && presetListIndex == 0) {
    presetListIndex = 2;
  }

  const char *labels[] = {"選択中と同じ構成",
                          "空（コンポーネントなし）",
                          "サンプル（球メッシュ+回転）",
                          "主カメラ（CmpCamera）"};
  constexpr int kPresetCount = 4;

  ImGui::SetNextItemWidth(240.0f);
  ImGui::BeginDisabled(!hasInstances && presetListIndex == 0);
  ImGui::Combo("追加テンプレート", &presetListIndex, labels, kPresetCount);
  ImGui::EndDisabled();

  const SceneObjectAddPreset preset = [&]() {
    switch (presetListIndex) {
    case 0:
      return SceneObjectAddPreset::CopySelection;
    case 1:
      return SceneObjectAddPreset::Empty;
    case 2:
      return SceneObjectAddPreset::StarterSample;
    case 3:
      return SceneObjectAddPreset::MainCamera;
    default:
      return SceneObjectAddPreset::StarterSample;
    }
  }();

  if (ImGui::Button("オブジェクトを追加")) {
    EntityEditorAddEmptyInstanceFromSelection::AppendInstance(editor, preset);
  }
  if (ImGui::IsItemHovered()) {
  ImGui::SetTooltip("テンプレートまたは選択中の定義を複製してルートに追加");
  }
}

} // namespace Atrum::EntityEditor::EntityEditorHierarchyAddObjectUi