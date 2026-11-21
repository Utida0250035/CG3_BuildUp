#include "System/Editor/EditorImportedAssetAutoStemRenameReport.h"

#include <cctype>
#include <filesystem>
#include <regex>

namespace Atrum::Editor::Import {

namespace fs = std::filesystem;

namespace {

bool MatchesStemIncrementPattern(const std::string &fileName,
                                 const std::string &originalStem,
                                 const std::string &ext) {
  static const std::regex pattern(R"(^(.+)_([0-9]+)(\..+)$)");
  std::smatch match;
  if (!std::regex_match(fileName, match, pattern)) {
    return false;
  }
  return match[1].str() == originalStem && match[3].str() == ext;
}

} // namespace

bool UsedAutoStemIncrementRename(const std::string &sourcePath,
                                 const std::string &destRelativePath) {
  if (sourcePath.empty() || destRelativePath.empty()) {
    return false;
  }
  const fs::path sourceFile = fs::path(sourcePath).filename();
  const fs::path destFile = fs::path(destRelativePath).filename();
  if (sourceFile == destFile) {
    return false;
  }
  return MatchesStemIncrementPattern(destFile.string(), sourceFile.stem().string(),
                                   sourceFile.extension().string());
}

std::string DescribeImportedCopyLine(const std::string &sourcePath,
                                     const std::string &destRelativePath) {
  const std::string destName = fs::path(destRelativePath).filename().string();
  if (UsedAutoStemIncrementRename(sourcePath, destRelativePath)) {
    return destRelativePath + "（自動リネーム stem_N: " + destName + "）";
  }
  if (fs::path(sourcePath).filename() != fs::path(destRelativePath).filename()) {
    return destRelativePath + "（リネーム後: " + destName + "）";
  }
  return destRelativePath;
}

} // namespace Atrum::Editor::Import