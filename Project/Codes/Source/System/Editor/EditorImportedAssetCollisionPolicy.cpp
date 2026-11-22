#include "System/Editor/EditorImportedAssetCollisionPolicy.h"

#include "System/Editor/EditorImportedAsset.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace Atrum::Editor::Import {

namespace fs = std::filesystem;

namespace {

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

std::string MakeIncrementedStemName(const fs::path &stem,
                                    const std::string &ext,
                                    const std::string &prefix) {
  for (int suffix = 1; suffix < 10000; ++suffix) {
    const std::string candidate =
        prefix + stem.string() + "_" + std::to_string(suffix) + ext;
    const fs::path dest = fs::path(kImportedAssetsDirectory) / candidate;
    if (!fs::exists(dest)) {
      return candidate;
    }
  }
  return {};
}

} // namespace

std::string ResolveImportedAssetDestinationFileName(
    const std::string &sourceFileName, const bool destinationExists,
    const EntityEditor::ImportedAssetNameCollisionPolicy policy,
    const std::string &batchNamePrefix) {
  const fs::path srcName = sourceFileName;
  const std::string fileName = srcName.filename().string();
  if (fileName.empty()) {
    return {};
  }
  if (!destinationExists) {
    return fileName;
  }

  const fs::path stem = srcName.stem();
  const std::string ext = srcName.extension().string();

  switch (policy) {
  case EntityEditor::ImportedAssetNameCollisionPolicy::OverwriteExisting:
    return fileName;
  case EntityEditor::ImportedAssetNameCollisionPolicy::TimestampSuffix:
    return stem.string() + "_" + FormatTimestampSuffix() + ext;
  case EntityEditor::ImportedAssetNameCollisionPolicy::BatchPrefixThenIncrement:
    return MakeIncrementedStemName(stem, ext, batchNamePrefix);
  case EntityEditor::ImportedAssetNameCollisionPolicy::AutoIncrementStem:
  default:
    return MakeIncrementedStemName(stem, ext, "");
  }
}

} // namespace Atrum::Editor::Import