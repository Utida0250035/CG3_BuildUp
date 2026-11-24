#include "System/Editor/Entity/EntityEditorProjectPanelSelectableLabel.h"

#include <filesystem>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include <imgui/imgui.h>
#endif

namespace Atrum::EntityEditor::ProjectPanelSelectableLabel {

std::string BuildPrefabSelectableLabel(const std::string &prefabPath) {
  const std::filesystem::path fsPath(prefabPath);
  std::string displayName = fsPath.stem().string();
  if (displayName.empty()) {
    displayName = fsPath.filename().string();
  }
  if (displayName.empty()) {
    displayName = prefabPath;
  }
  return displayName + "##" + prefabPath;
}

std::string BuildImportedAssetSelectableLabel(const std::string &assetPath) {
  const std::filesystem::path fsPath(assetPath);
  std::string displayName = fsPath.filename().string();
  if (displayName.empty()) {
    displayName = assetPath;
  }
  return displayName + "##" + assetPath;
}

void DrawFullPathHoverTooltip(const std::string &fullPath) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (fullPath.empty()) {
    return;
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", fullPath.c_str());
  }
#else
  (void)fullPath;
#endif
}

} // namespace Atrum::EntityEditor::ProjectPanelSelectableLabel
