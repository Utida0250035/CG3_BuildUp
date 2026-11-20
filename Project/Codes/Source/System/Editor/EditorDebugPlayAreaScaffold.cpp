#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorDebugPlayAreaScaffold.h"

#include "System/Editor/DebugPlayAreaScaffoldGuidance.h"
#include "System/Editor/DebugPlayAreaScaffoldPersonalize.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "Scene/SceneEntityEditorInEditorPlayQueue.h"
#include "Scene/SceneEntityEditorScenePlayBuildUpRoute.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "ForDebug/ImGui.h"
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace Atrum::Editor {

namespace fs = std::filesystem;

DebugPlayAreaScaffoldResult
EditorDebugPlayAreaScaffold::GenerateFromTemplate(const std::string &sceneName,
                                                  const bool forceOverwrite) {
  DebugPlayAreaScaffoldResult result{};
  if (sceneName.empty()) {
    result.message = "シーン名を入力してください";
    return result;
  }

  const fs::path templatePath =
      fs::path(EditorAssets::kDebugPlayAreaTemplateScene);
  if (!fs::is_regular_file(templatePath)) {
    result.message = "テンプレが見つかりません: " + templatePath.generic_string();
    return result;
  }

  const std::string fileStem = SanitizeDebugPlayAreaFileStem(sceneName);
  const fs::path outDir = fs::path(EditorAssets::kDebugPlayAreaDirectory);
  const fs::path outPath = outDir / (fileStem + ".scene.json");

  if (fs::exists(outPath) && !forceOverwrite) {
    result.message = "既存ファイルがあります（上書きするには再度実行）: " +
                     outPath.generic_string();
    return result;
  }

  std::ifstream in(templatePath);
  if (!in) {
    result.message = "テンプレを開けません";
    return result;
  }

  nlohmann::json data = nlohmann::json::parse(in, nullptr, false);
  if (data.is_discarded()) {
    result.message = "テンプレ JSON のパースに失敗しました";
    return result;
  }

  PersonalizeDebugPlayAreaSceneJson(data, sceneName);

  std::error_code ec{};
  fs::create_directories(outDir, ec);
  std::ofstream out(outPath);
  if (!out) {
    result.message = "出力を開けません: " + outPath.generic_string();
    return result;
  }
  out << data.dump(2) << '\n';

  result.success = true;
  result.outputPath = outPath.generic_string();
  result.message = DebugPlayAreaScaffoldGuidance::kPostGenerateStepsJa;
  return result;
}

void EditorDebugPlayAreaScaffold::DrawProjectPanelControls(
    EntityEditor::EntityEditor &editor) {
  static char nameBuffer[96] = "FeatureTest";
  ImGui::InputText("テストエリア名", nameBuffer, sizeof(nameBuffer));
  if (ImGui::Button("テストエリア作成 (テンプレ)")) {
    const DebugPlayAreaScaffoldResult gen =
        GenerateFromTemplate(nameBuffer, false);
    editor.Ui().importStatusMessage = gen.message;
    if (gen.success) {
      editor.OpenSceneFromPath(gen.outputPath);
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("作成して BuildUp")) {
    const DebugPlayAreaScaffoldResult gen =
        GenerateFromTemplate(nameBuffer, false);
    if (gen.success) {
      editor.OpenSceneFromPath(gen.outputPath);
      EntityEditor::QueueEditorScenePlayViaBuildUp(editor.Ui());
      editor.Ui().importStatusMessage =
          DebugPlayAreaScaffoldGuidance::kPostGenerateStepsJa;
    } else {
      editor.Ui().importStatusMessage = gen.message;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("作成してエディタ内プレビュー")) {
    const DebugPlayAreaScaffoldResult gen =
        GenerateFromTemplate(nameBuffer, false);
    if (gen.success) {
      editor.OpenSceneFromPath(gen.outputPath);
      EntityEditor::QueueInEditorPlayRuntimeTabOnly(editor.Ui());
      editor.Ui().importStatusMessage =
          DebugPlayAreaScaffoldGuidance::kPostGenerateStepsJa;
    } else {
      editor.Ui().importStatusMessage = gen.message;
    }
  }
  ImGui::TextDisabled("(DebugPlayArea)");
}

} // namespace Atrum::Editor

#endif