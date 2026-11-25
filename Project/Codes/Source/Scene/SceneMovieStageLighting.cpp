#include "Scene/SceneMovieStageLighting.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
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
}

} // namespace Atrum