#include "System/Editor/EditorExternalAssetOpen.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <shellapi.h>
#include <windows.h>
#endif

namespace Atrum::Editor {

namespace fs = std::filesystem;

namespace {

std::string ExtensionLower(const std::string &path) {
  const fs::path filePath(path);
  std::string ext = filePath.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](const unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  return ext;
}

} // namespace

bool IsExternalAssetEditorTargetExtension(const std::string &path) {
  const std::string ext = ExtensionLower(path);
  return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".obj" ||
         ext == ".gltf" || ext == ".glb";
}

std::string ResolveExistingProjectAssetFile(const std::string &path) {
  if (path.empty()) {
    return {};
  }
  std::error_code ec;
  const fs::path absolute = fs::absolute(fs::path(path), ec).lexically_normal();
  if (ec || !fs::is_regular_file(absolute, ec) || ec) {
    return {};
  }
  try {
    return fs::weakly_canonical(absolute).generic_string();
  } catch (...) {
    return absolute.generic_string();
  }
}

bool TryOpenExternalAssetWithDefaultApp(const std::string &projectRelativePath,
                                        std::string &outError) {
  outError.clear();
  if (projectRelativePath.empty()) {
    outError = "パスが空です";
    return false;
  }
  if (!IsExternalAssetEditorTargetExtension(projectRelativePath)) {
    outError = "外部ツールで開く対象の拡張子ではありません";
    return false;
  }
  const std::string absolute =
      ResolveExistingProjectAssetFile(projectRelativePath);
  if (absolute.empty()) {
    outError = "ファイルが見つかりません: " + projectRelativePath;
    return false;
  }

#if defined(_WIN32)
  const HINSTANCE result =
      ShellExecuteA(nullptr, "open", absolute.c_str(), nullptr, nullptr,
                    SW_SHOWNORMAL);
  const intptr_t code = reinterpret_cast<intptr_t>(result);
  if (code <= 32) {
    outError = "ShellExecute に失敗しました (code=" + std::to_string(code) + ")";
    return false;
  }
  return true;
#elif defined(__APPLE__)
  const int status = std::system(
      ("open " + std::string("\"") + absolute + "\"").c_str());
  if (status != 0) {
    outError = "open コマンドに失敗しました";
    return false;
  }
  return true;
#else
  const int status = std::system(
      ("xdg-open " + std::string("\"") + absolute + "\"").c_str());
  if (status != 0) {
    outError = "xdg-open に失敗しました";
    return false;
  }
  return true;
#endif
}

} // namespace Atrum::Editor