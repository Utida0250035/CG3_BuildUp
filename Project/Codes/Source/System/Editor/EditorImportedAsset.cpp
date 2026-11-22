#include "System/Editor/EditorImportedAsset.h"

#include "System/Editor/EditorImportedAssetCollisionPolicy.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <filesystem>

namespace Atrum::Editor::Import {

namespace fs = std::filesystem;

namespace {

EntityEditor::ImportedAssetNameCollisionPolicy &ActiveCollisionPolicy() {
  static EntityEditor::ImportedAssetNameCollisionPolicy policy =
      EntityEditor::ImportedAssetNameCollisionPolicy::AutoIncrementStem;
  return policy;
}

std::string &ActiveBatchPrefix() {
  static std::string prefix;
  return prefix;
}

} // namespace

void SetImportedAssetCollisionPolicyForNextCopies(
    const EntityEditor::ImportedAssetNameCollisionPolicy policy) {
  ActiveCollisionPolicy() = policy;
}

void SetImportedAssetBatchNamePrefixForNextCopies(const std::string &prefix) {
  ActiveBatchPrefix() = prefix;
}

EntityEditor::ImportedAssetNameCollisionPolicy
GetImportedAssetCollisionPolicyForCopies() {
  return ActiveCollisionPolicy();
}

FileDropQueue &FileDropQueue::Instance() {
  static FileDropQueue queue;
  return queue;
}

void FileDropQueue::Push(std::string path) {
  if (path.empty()) {
    return;
  }
  pending_.push_back(std::move(path));
}

std::vector<std::string> FileDropQueue::Drain() {
  std::vector<std::string> out = std::move(pending_);
  pending_.clear();
  return out;
}

void EnsureImportedAssetsDirectoryExists() {
  const fs::path root = kImportedAssetsDirectory;
  if (!fs::exists(root)) {
    fs::create_directories(root);
  }
}

std::string CopyFileIntoImportedAssets(const std::string &sourcePath) {
  namespace fs = std::filesystem;
  const fs::path src = sourcePath;
  if (!fs::exists(src) || !fs::is_regular_file(src)) {
    return {};
  }

  EnsureImportedAssetsDirectoryExists();

  const std::string fileName = src.filename().string();
  if (fileName.empty()) {
    return {};
  }

  auto makeDestination = [&](const std::string &name) {
    return fs::path(kImportedAssetsDirectory) / name;
  };

  fs::path dest = makeDestination(fileName);
  const std::string destFileName = ResolveImportedAssetDestinationFileName(
      fileName, fs::exists(dest), ActiveCollisionPolicy(), ActiveBatchPrefix());
  if (destFileName.empty()) {
    return {};
  }
  dest = makeDestination(destFileName);

  std::error_code ec;
  fs::copy_file(src, dest, fs::copy_options::overwrite_existing, ec);
  if (ec) {
    return {};
  }

  return dest.generic_string();
}

} // namespace Atrum::Editor::Import