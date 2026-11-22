#include "Scene/SceneMoviePendingTransitionUpdate.h"

#include "Camera/CameraController/MovieCameraController.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void TickSceneMovieVisualsDuringPendingTransition(
    const bool trackPlaybackStarted, MovieCameraController &movieCamera,
    Entity *skyDome) {
  if (trackPlaybackStarted) {
    movieCamera.Update();
  }
  if (skyDome != nullptr && skyDome->IsActive()) {
    skyDome->Update();
  }
}

} // namespace Atrum