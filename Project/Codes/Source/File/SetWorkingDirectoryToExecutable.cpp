#include "File/SetWorkingDirectoryToExecutable.h"

#include <Windows.h>
#include <filesystem>

namespace Atrum::File {

namespace {

#if defined(DEVELOPMENT)

// exe から上方向に辿り、リポジトリ内の Project/Assets を見つける
std::filesystem::path FindProjectContentRoot(
    const std::filesystem::path &executableDirectory) {
  namespace fs = std::filesystem;

  constexpr const char *kAssetsMarker =
      "Assets/Images/white4x4.png";

  fs::path probe = executableDirectory;
  for (int depth = 0; depth < 8; ++depth) {
    const fs::path projectAssets = probe / "Project" / kAssetsMarker;
    if (fs::exists(projectAssets)) {
      return probe / "Project";
    }

    if (!probe.has_parent_path() || probe.parent_path() == probe) {
      break;
    }
    probe = probe.parent_path();
  }

  return {};
}

#endif

} // namespace

bool SetWorkingDirectoryToExecutableDirectory() {
  wchar_t modulePath[MAX_PATH]{};
  const DWORD length = GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
  if (length == 0 || length >= MAX_PATH) {
    MessageBoxW(nullptr,
                L"実行ファイルのパスを取得できませんでした。",
                L"CG2 BuildUp", MB_OK | MB_ICONERROR);
    return false;
  }

  std::error_code errorCode;
  const std::filesystem::path executableDirectory =
      std::filesystem::path(modulePath).parent_path();
  std::filesystem::current_path(executableDirectory, errorCode);
  if (errorCode) {
    MessageBoxW(nullptr,
                L"作業フォルダを実行ファイルの場所に設定できませんでした。",
                L"CG2 BuildUp", MB_OK | MB_ICONERROR);
    return false;
  }

#if defined(DEVELOPMENT)
  // 開発中は Project/Assets を直接読む（JSON 編集が PostBuild なしでも反映される）
  const std::filesystem::path projectContentRoot =
      FindProjectContentRoot(executableDirectory);
  if (!projectContentRoot.empty()) {
    std::filesystem::current_path(projectContentRoot, errorCode);
    if (errorCode) {
      MessageBoxW(nullptr,
                  L"作業フォルダを Project に設定できませんでした。",
                  L"CG2 BuildUp", MB_OK | MB_ICONERROR);
      return false;
    }
  }
#endif

  return true;
}

} // namespace Atrum::File
