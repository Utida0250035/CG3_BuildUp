#include "ForDebug/ImGuiJapaneseFont.h"

#ifdef USE_IMGUI

#include <imgui/imgui.h>
#include <filesystem>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace Atrum::Debug {

namespace {

#if defined(_WIN32)
std::string WidePathToUtf8(const std::wstring &widePath) {
  if (widePath.empty()) {
    return {};
  }
  const int byteCount =
      WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, nullptr, 0, nullptr,
                          nullptr);
  if (byteCount <= 0) {
    return {};
  }
  std::string utf8(static_cast<size_t>(byteCount - 1), '\0');
  WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, utf8.data(), byteCount,
                      nullptr, nullptr);
  return utf8;
}
#endif

bool TryAddFontFromPath(ImGuiIO &io, const std::string &fontPathUtf8,
                        const float sizePixels,
                        const ImWchar *glyphRanges) {
  namespace fs = std::filesystem;
  if (!fs::exists(fontPathUtf8)) {
    return false;
  }
  ImFontConfig fontConfig{};
  fontConfig.OversampleH = 2;
  fontConfig.OversampleV = 1;
  ImFont *font = io.Fonts->AddFontFromFileTTF(
      fontPathUtf8.c_str(), sizePixels, &fontConfig, glyphRanges);
  return font != nullptr;
}

} // namespace

void SetupImGuiJapaneseFont(ImGuiIO &io) {
  io.Fonts->Clear();

  const ImWchar *glyphRanges = io.Fonts->GetGlyphRangesJapanese();
  constexpr float kFontSize = 18.0f;

  const std::vector<std::string> projectRelativeCandidates = {
      "Assets/Fonts/NotoSansJP-Regular.otf",
      "Assets/Fonts/NotoSansCJKjp-Regular.otf",
      "../EditorAssets/Fonts/NotoSansJP-Regular.otf",
  };

  for (const std::string &path : projectRelativeCandidates) {
    if (TryAddFontFromPath(io, path, kFontSize, glyphRanges)) {
      return;
    }
  }

#if defined(_WIN32)
  wchar_t windowsDir[MAX_PATH]{};
  if (GetWindowsDirectoryW(windowsDir, MAX_PATH) > 0) {
    const wchar_t *fontSuffixes[] = {
        L"\\Fonts\\meiryo.ttc",
        L"\\Fonts\\YuGothM.ttc",
        L"\\Fonts\\msgothic.ttc",
        L"\\Fonts\\segoeui.ttf",
    };
    for (const wchar_t *suffix : fontSuffixes) {
      const std::string pathUtf8 =
          WidePathToUtf8(std::wstring(windowsDir) + suffix);
      if (TryAddFontFromPath(io, pathUtf8, kFontSize, glyphRanges)) {
        return;
      }
    }
  }
#endif

  io.Fonts->AddFontDefault();
}

} // namespace Atrum::Debug

#endif
