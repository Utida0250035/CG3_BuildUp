#include "Visual/PlayerTerrainSilhouette.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/PlayerParryReach.h"
#include "System/WhiteTexture.h"

namespace Atrum::PlayerTerrainSilhouette {

void DrawOccludedSilhouette(AtrumEngine *engine,
                            const Math::Matrix4x4 &playerWorldMatrix,
                            const float sphereRadius) {
  if (engine == nullptr) {
    return;
  }

  const float radius =
      sphereRadius > 0.0f ? sphereRadius : kParryColliderRadius;
  const Math::Vector4 silhouetteColor{0.35f, 0.9f, 1.0f, 0.58f};

  engine->SetBlendModeForFrame(BlendMode::OCCLUDED_SILHOUETTE);
  engine->DrawSphere(WhiteTextureIndex(), silhouetteColor, Math::Transform{},
                     playerWorldMatrix, radius, 12, false);
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum::PlayerTerrainSilhouette
