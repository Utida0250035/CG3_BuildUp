#include "System/Build/BuildConfigurationContract.h"

namespace Atrum::Build {

#if defined(NDEBUG) && !defined(DEVELOPMENT) && !defined(USE_IMGUI) && \
    !defined(_DEBUG) && !defined(MSVC_DEBUG_CONFIGURATION)
// Release|x64 … 出荷候補
#elif defined(DEVELOPMENT) && defined(USE_IMGUI) && \
    !defined(_DEBUG) && !defined(MSVC_DEBUG_CONFIGURATION)
// Development|x64 アプリケーション開発(非出荷)

#elif defined(_DEBUG) && defined(USE_IMGUI) && \
    defined(MSVC_DEBUG_CONFIGURATION)
// Debug|x64 … MSVC Debug CRT + エディタ（非出荷・CI DebugBuild）
#else
#error \
    "未登録のプリプロセッサ構成です。Project/CG2_BuildUp.vcxproj と ship_build_configurations.md を確認してください。"
#endif

const char *ConfigurationLabel() {
#if defined(NDEBUG) && !defined(DEVELOPMENT) && !defined(USE_IMGUI)
  return "Release|x64 (出荷候補)";
#elif defined(NDEBUG) && defined(DEVELOPMENT) && defined(USE_IMGUI)
  return "Development|x64 (エディタ日常)";
#elif defined(_DEBUG) && defined(DEVELOPMENT) && defined(USE_IMGUI) && \
    defined(MSVC_DEBUG_CONFIGURATION)
  return "Debug|x64 (MSVC Debug CRT・非出荷)";
#else
  return "Unknown";
#endif
}

bool IsShipCandidateBuild() {
#if defined(NDEBUG) && !defined(DEVELOPMENT) && !defined(USE_IMGUI) && \
    !defined(_DEBUG)
  return true;
#else
  return false;
#endif
}

} // namespace Atrum::Build