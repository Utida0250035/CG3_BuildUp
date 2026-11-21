#include "System/Editor/Prefab/PrefabAssetAutoStemRenameReport.h"

#include <cctype>
#include <filesystem>

namespace Atrum::Editor::Prefab::PrefabAssetAutoStemRenameReport {

namespace {

std::string SanitizeStem(std::string stem) {
  if (stem.empty()) {
    stem = "Entity";
  }
  for (char &ch : stem) {
    if (ch == '/' || ch == '\\' || ch == ':' || ch == '"' || ch == '*' ||
        ch == '?' || ch == '<' || ch == '>' || ch == '|') {
      ch = '_';
    }
  }
  return stem;
}

} // namespace

bool UsedAutoStemIncrementRename(const std::string &baseName,
                                 const std::string &outputPath) {
  if (outputPath.empty()) {
    return false;
  }
  const std::string stem = SanitizeStem(baseName);
  const std::string expectedPrimary = stem + ".json";
  const std::string destName =
      std::filesystem::path(outputPath).filename().string();
  if (destName == expectedPrimary) {
    return false;
  }
  if (destName.size() <= stem.size() + 6) {
    return false;
  }
  if (destName.compare(0, stem.size(), stem) != 0 ||
      destName[stem.size()] != '_' ||
      destName.compare(destName.size() - 5, 5, ".json") != 0) {
    return false;
  }
  const size_t numStart = stem.size() + 1;
  const size_t numEnd = destName.size() - 5;
  if (numEnd <= numStart) {
    return false;
  }
  for (size_t index = numStart; index < numEnd; ++index) {
    if (!std::isdigit(static_cast<unsigned char>(destName[index]))) {
      return false;
    }
  }
  return true;
}

} // namespace Atrum::Editor::Prefab::PrefabAssetAutoStemRenameReport