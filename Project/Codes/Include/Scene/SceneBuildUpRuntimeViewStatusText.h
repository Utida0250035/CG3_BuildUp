#pragma once

#include <string>

namespace Atrum {

class SceneBuildUp;

/// 専用ランタイムビュー見出し用（シーン名と JSON パスを含む）
std::string BuildSceneBuildUpRuntimeViewStatusLine(const SceneBuildUp &scene);

} // namespace Atrum