#pragma once

#include <cstddef>
#include <string>

namespace Atrum::AtomF {

/// SceneBuildUp が読み込んだ JSON パスが atomF シーンかどうか。
inline bool PathEndsWithAtomFJson(const std::string &sceneJsonPath) {
  constexpr const char *kFileName = "atomF.json";
  constexpr std::size_t kFileNameLength = 10;
  if (sceneJsonPath.size() < kFileNameLength) {
    return false;
  }
  return sceneJsonPath.compare(sceneJsonPath.size() - kFileNameLength,
                               kFileNameLength, kFileName) == 0;
}

} // namespace Atrum::AtomF