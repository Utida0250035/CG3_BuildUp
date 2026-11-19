#include "Scene/SceneMovieStageLighting.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Renderer/Lighting/GodRays.h"
#include "Engine/Renderer/Lighting/VolumetricFog.h"
#include "Engine/Resource/LightData.h"

namespace Atrum {

void ApplySceneMovieStageLighting() {
  Math::Vector3 lightDirection{-0.4f, -0.85f, 0.15f};
  lightDirection.Normalize();
  const DirectionalLightData directionalLight{
      .color = Math::Vector3{1.0f, 1.0f, 1.0f},
      .direction = lightDirection,
      .intensity = 8.0f,
      .lightModel = LightModel::HalfLambert,
      .rimColor = Math::Vector3{0.78f, 0.86f, 1.0f},
      .rimIntensity = 0.32f,
      .rimPower = 3.2f,
      .rimEnabled = 1u,
  };
  ::Atrum::EngineSetDirectionalLightData(directionalLight);

  Engine *const engine = Engine::Instance();
  if (engine == nullptr) {
    return;
  }

  Visual::VolumetricFogSettings movieFog{};
  movieFog.enabled = 1u;
  movieFog.density = 0.018f;
  movieFog.heightFalloff = 0.26f;
  movieFog.heightOffset = 0.0f;
  movieFog.maxDistance = 96.0f;
  movieFog.inScatterIntensity = 0.16f;
  engine->RefVolumetricFog().SetSettings(movieFog);

  Visual::GodRaySettings movieGodRays{};
  movieGodRays.enabled = 1u;
  movieGodRays.intensity = 0.1f;
  movieGodRays.angularPower = 7.5f;
  movieGodRays.fogCoupling = 0.85f;
  engine->RefGodRays().SetSettings(movieGodRays);
}

} // namespace Atrum