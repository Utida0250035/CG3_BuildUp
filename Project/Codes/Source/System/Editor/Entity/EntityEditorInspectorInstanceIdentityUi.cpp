#include "System/Editor/Entity/EntityEditorInspectorInstanceIdentityUi.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <imgui.h>

namespace Atrum::EntityEditor::EntityEditorInspectorInstanceIdentityUi {

bool Draw(EntityEditor &editor, SceneEntityInstance &instance,
          EntityDefinition &definition, bool &outChanged) {
  outChanged = false;
  (void)editor;

  ImGui::TextUnformatted("名前（シーンとランタイムで別キー）");
  ImGui::TextDisabled(
      "ヒエラルキー＝instanceName / ランタイム生成＝definition.entityName");
  if (instance.instanceName != definition.entityName) {
    ImGui::TextDisabled("※ 2 つの名前が異なります（プレビューは instanceName を優先）");
  }

  const bool instanceNameEdited =
      ImGui::InputString("ヒエラルキー表示名 (instanceName)",
                         instance.instanceName);
  if (instanceNameEdited) {
    outChanged = true;
  }

  const bool entityNameEdited =
      ImGui::InputString("エンティティ名 (entityName)", definition.entityName);
  if (entityNameEdited) {
    outChanged = true;
  }

  ImGui::BeginDisabled(
      instance.instanceName == definition.entityName);
  if (ImGui::SmallButton("表示名 → エンティティ名")) {
    definition.entityName = instance.instanceName;
    outChanged = true;
  }
  ImGui::SameLine();
  if (ImGui::SmallButton("エンティティ名 → 表示名")) {
    instance.instanceName = definition.entityName;
    outChanged = true;
  }
  ImGui::EndDisabled();

  return outChanged;
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorInstanceIdentityUi