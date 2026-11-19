#pragma once

#include <string>

namespace Atrum {

/// トラック JSON 読込失敗時の表示用メッセージ（要求パスと解決パスを含む）
std::string BuildSceneMovieTrackLoadFailureMessage(const std::string &requestedPath);

/// 自動復帰までの最低表示時間（秒）
constexpr float kSceneMovieTrackLoadFailureHoldSeconds = 2.5f;

} // namespace Atrum