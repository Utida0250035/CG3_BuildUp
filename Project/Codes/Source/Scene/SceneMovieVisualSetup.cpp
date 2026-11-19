#include "Scene/SceneMovieVisualSetup.h"

#include "Camera/Camera.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"

#include <memory>

namespace Atrum {

std::unique_ptr<Entity> CreateSkyDomeBoundToCamera(Camera *camera) {
  if (camera == nullptr) {
    return {};
  }
  return EntityFactory::SkyDome(camera);
}

} // namespace Atrum