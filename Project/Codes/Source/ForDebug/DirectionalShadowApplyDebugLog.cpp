#include "ForDebug/DirectionalShadowApplyDebugLog.h"

#if defined(DEVELOPMENT)

#include "ForDebug/Log.h"

#include <cstdio>
#include <string>

namespace Atrum::Debug {

namespace {

void LogLine(const std::string &line) { LogFile::Instance()->Log(line); }

} // namespace

void AppendDirectionalShadowApplySnapshotToLog(
    const Visual::DirectionalShadowSettings &settings,
    const Visual::DirectionalShadowApplyDiagnostics::ApplySnapshot &snapshot) {
  LogLine("[DirectionalShadow] snapshot begin");
  LogLine(std::string("[DirectionalShadow] shadeEnabled=") +
          std::to_string(settings.shadeEnabled) +
          " planarEnabled=" + std::to_string(settings.planarEnabled));
  LogLine(std::string("[DirectionalShadow] shadeDarkness=") +
          std::to_string(settings.shadeDarkness) + " groundPlaneY=" +
          std::to_string(settings.groundPlaneY) + " contactHeight=" +
          std::to_string(settings.contactHeight));
  LogLine(std::string("[DirectionalShadow] planarAlpha=") +
          std::to_string(settings.planarAlpha) + " planarSquash=" +
          std::to_string(settings.planarSquash) + " planarRadiusScale=" +
          std::to_string(settings.planarRadiusScale));

  const bool castOn =
      settings.shadeEnabled != 0u || settings.planarEnabled != 0u;
  LogLine(std::string("[DirectionalShadow] CAST_SHADOW=") + (castOn ? "1" : "0"));

  char counters[256];
  std::snprintf(
      counters, sizeof(counters),
      "[DirectionalShadow] planarDraw=%llu planarSkip=%llu lightMerge=%llu "
      "lastSkip=%s",
      static_cast<unsigned long long>(snapshot.planarDrawCount),
      static_cast<unsigned long long>(snapshot.planarSkipCount),
      static_cast<unsigned long long>(snapshot.lightDataMergeCount),
      Visual::DirectionalShadowApplyDiagnostics::PlanarDrawSkipReasonLabel(
          snapshot.lastPlanarSkip));
  LogLine(counters);
  LogLine("[DirectionalShadow] snapshot end");
}

} // namespace Atrum::Debug

#endif