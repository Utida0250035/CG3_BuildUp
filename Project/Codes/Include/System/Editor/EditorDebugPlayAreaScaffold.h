#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include <string>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::Editor {

struct DebugPlayAreaScaffoldResult {
  bool success = false;
  std::string outputPath{};
  std::string message{};
};

/// template_test_play_area から DebugPlayArea 配下へシーン JSON を生成する。
class EditorDebugPlayAreaScaffold {
public:
  static DebugPlayAreaScaffoldResult
  GenerateFromTemplate(const std::string &sceneName, bool forceOverwrite = false);

  static void DrawProjectPanelControls(EntityEditor::EntityEditor &editor);
};

} // namespace Atrum::Editor

#endif