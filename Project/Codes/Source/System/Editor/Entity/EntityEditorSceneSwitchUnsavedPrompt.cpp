#include "System/Editor/Entity/EntityEditorSceneSwitchUnsavedPrompt.h"

#include "System/Editor/Entity/EntityEditor.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "ForDebug/ImGui.h"

namespace Atrum::EntityEditor {

namespace {

constexpr const char *kSceneSwitchUnsavedPopupId = "シーン切替（未保存）";

} // namespace

void DrawEntityEditorSceneSwitchUnsavedPrompt(EntityEditor &editor) {
  UiState &ui = editor.Ui();

  if (ui.requestSceneSwitchUnsavedPrompt) {
    ImGui::OpenPopup(kSceneSwitchUnsavedPopupId);
    ui.requestSceneSwitchUnsavedPrompt = false;
  }

  if (!ImGui::BeginPopupModal(kSceneSwitchUnsavedPopupId, nullptr,
                              ImGuiWindowFlags_AlwaysAutoResize)) {
    return;
  }

  ImGui::TextUnformatted("現在のシーンに未保存の変更があります。");
  ImGui::Spacing();
  ImGui::TextWrapped("編集中: %s", ui.jsonFilePath.c_str());
  ImGui::TextWrapped("開く先: %s", ui.pendingSceneSwitchPath.c_str());
  ImGui::Spacing();

  if (ImGui::Button("保存して開く", ImVec2(140.0f, 0.0f))) {
    editor.SaveToJsonPath(ui.jsonFilePath);
    const std::string next = ui.pendingSceneSwitchPath;
    ui.pendingSceneSwitchPath.clear();
    editor.OpenSceneFromPath(next);
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("保存せず開く", ImVec2(140.0f, 0.0f))) {
    const std::string next = ui.pendingSceneSwitchPath;
    ui.pendingSceneSwitchPath.clear();
    editor.OpenSceneFromPath(next);
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("キャンセル", ImVec2(120.0f, 0.0f))) {
    ui.pendingSceneSwitchPath.clear();
    ImGui::CloseCurrentPopup();
  }

  ImGui::EndPopup();
}

} // namespace Atrum::EntityEditor

#else

namespace Atrum::EntityEditor {

void DrawEntityEditorSceneSwitchUnsavedPrompt(EntityEditor &) {}

} // namespace Atrum::EntityEditor

#endif