#include "Scene/SceneMovieTrackFailure.h"

#include "Camera/MovieCameraTrackPathResolve.h"

namespace Atrum {

std::string BuildSceneMovieTrackLoadFailureMessage(
    const std::string &requestedPath) {
  const std::string resolvedPath =
      ResolveMovieCameraTrackJsonPath(requestedPath);
  return "ムービートラックの読込に失敗しました\n要求パス: " + requestedPath +
         "\n解決パス: " + resolvedPath +
         "\nEsc で戻る / 短い待機後に自動復帰します";
}

} // namespace Atrum