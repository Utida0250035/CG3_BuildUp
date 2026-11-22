#pragma once

#include <filesystem>
#include <string>

namespace Atrum::Editor::Scene {

/// エディタが一覧・読込対象とするシーン JSON（`*.json` / `*.scene.json`）
inline bool IsEditableSceneJsonPath(const std::filesystem::path &path) {
  if (!path.has_filename()) {
    return false;
  }
  const std::string filename = path.filename().string();
  if (filename.size() >= 12 &&
      filename.compare(filename.size() - 12, 12, ".scene.json") == 0) {
    return true;
  }
  return path.extension() == ".json";
}

} // namespace Atrum::Editor::Scene