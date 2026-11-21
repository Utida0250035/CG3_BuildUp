#include "System/Editor/Prefab/PrefabAssetCollisionPolicy.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Prefab/PrefabAsset.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace Atrum::Editor::Prefab {

namespace fs = std::filesystem;

namespace {

std::string SanitizePrefabStem(std::string stem) {
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

std::string FormatTimestampSuffix() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm localTime{};
#if defined(_WIN32)
  localtime_s(&localTime, &t);
#else
  localtime_r(&t, &localTime);
#endif
  std::ostringstream oss;
  oss << std::put_time(&localTime, "%Y%m%d_%H%M%S");
  return oss.str();
}

std::string MakeIncrementedStemJsonName(const std::string &stem,
                                        const std::string &prefix) {
  for (int suffix = 1; suffix < 10000; ++suffix) {
    const std::string fileName =
        prefix + stem + "_" + std::to_string(suffix) + ".json";
    const fs::path dest = fs::path(kEntitiesPrefabDirectory) / fileName;
    if (!fs::exists(dest)) {
      return fileName;
    }
  }
  return {};
}

std::string BuildPathForFileName(const std::string &fileName) {
  if (fileName.empty()) {
    return {};
  }
  return (fs::path(kEntitiesPrefabDirectory) / fileName).generic_string();
}

} // namespace

std::string ResolvePrefabJsonOutputPath(
    const std::string &baseName,
    const EntityEditor::PrefabAssetNameCollisionPolicy policy,
    const std::string &batchNamePrefix) {
  EnsureEntitiesPrefabDirectoryExists();
  const std::string stem = SanitizePrefabStem(baseName);

  const fs::path primary = fs::path(kEntitiesPrefabDirectory) / (stem + ".json");
  const bool exists = fs::exists(primary);

  if (!exists) {
    return primary.generic_string();
  }

  switch (policy) {
  case EntityEditor::PrefabAssetNameCollisionPolicy::OverwriteExisting:
    return primary.generic_string();
  case EntityEditor::PrefabAssetNameCollisionPolicy::TimestampSuffix:
    return BuildPathForFileName(stem + "_" + FormatTimestampSuffix() + ".json");
  case EntityEditor::PrefabAssetNameCollisionPolicy::BatchPrefixThenIncrement: {
    const std::string fileName =
        MakeIncrementedStemJsonName(stem, batchNamePrefix);
    return BuildPathForFileName(fileName);
  }
  case EntityEditor::PrefabAssetNameCollisionPolicy::AutoIncrementStem:
  default: {
    const std::string fileName = MakeIncrementedStemJsonName(stem, "");
    return BuildPathForFileName(fileName);
  }
  }
}

} // namespace Atrum::Editor::Prefab