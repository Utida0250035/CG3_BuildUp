#include "System/Editor/EditorImportedAsset.h"

#include <filesystem>

namespace Atrum::Editor::Import {

namespace fs = std::filesystem;

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
  if (fs::exists(dest)) {
    const fs::path stem = src.stem();
    const std::string ext = src.extension().string();
    bool copied = false;
    for (int suffix = 1; suffix < 10000; ++suffix) {
      dest = makeDestination(stem.string() + "_" + std::to_string(suffix) + ext);
      if (!fs::exists(dest)) {
        copied = true;
        break;
      }
    }
    if (!copied) {
      return {};
    }
  }

  std::error_code ec;
  fs::copy_file(src, dest, fs::copy_options::overwrite_existing, ec);
  if (ec) {
    return {};
  }

  return dest.generic_string();
}

} // namespace Atrum::Editor::Import
