#pragma once

#include <string>

namespace Atrum {

/// BuildUp 用シーン JSON を cwd 非依存で解決する（配布 exe の作業ディレクトリ差を吸収）
std::string ResolveSceneBuildUpJsonPath(const std::string &path);

} // namespace Atrum