#include "System/Entity/Components/CmpMeshSphere.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "System/BulletDepthVisual.h"
#include "System/Entity/Entity.h"
#include "System/StageBounds.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpMeshSphere::Draw() {
  AtrumEngine *engine = AtrumEngine::Instance();
  const Entity *owner = GetOwner();
  const float scale = RefOwner().GetTransform().scale.x;
  float drawRadius = radius_ * scale;
  Math::Vector4 drawColor = colorVec_;

  const bool isDepthBullet =
      owner != nullptr &&
      BulletDepthVisual::IsManagedBulletEntityType(owner->GetEntityType());

  float pseudoDepth = 0.0f;
  if (isDepthBullet) {
    const float worldY = owner->GetWorldPosition().y;
    pseudoDepth = BulletDepthVisual::PseudoDepthLane01FromWorldY(worldY);
    drawRadius *= BulletDepthVisual::DrawScaleFromPseudoDepth(pseudoDepth);
    drawColor = BulletDepthVisual::ModulateBulletColor(
        colorVec_, pseudoDepth, owner->GetEntityType());
    drawColor.w *= BulletDepthVisual::BodyAlphaFromPseudoDepth(pseudoDepth);
  }

  if (isDepthBullet) {
    const Math::Vector3 worldPos = owner->GetWorldPosition();

    Math::TransformLH shadowTransform{};
    shadowTransform.translate = {worldPos.x,
                                 BulletDepthVisual::GroundShadowCenterWorldY(),
                                 worldPos.z};
    shadowTransform.scale = {1.0f, BulletDepthVisual::GroundShadowVerticalSquash(),
                             1.0f};

    engine->SetBlendModeForFrame(BlendMode::NORMAL);
    engine->DrawSphere(
        WhiteTextureIndex(),
        BulletDepthVisual::GroundShadowColor(pseudoDepth), Math::Transform{},
        shadowTransform.MakeWorldMatrix(),
        drawRadius * BulletDepthVisual::GroundShadowRadiusScale(pseudoDepth), 2,
        false);

    engine->SetBlendModeForFrame(BlendMode::ADD);
    engine->DrawSphere(
        WhiteTextureIndex(),
        BulletDepthVisual::GlowColorFromBody(drawColor, pseudoDepth),
        Math::Transform{}, owner->GetWorldMatrix(),
        drawRadius * BulletDepthVisual::GlowRadiusScale(pseudoDepth), 4, false);
  }

  const bool useAlphaBlend = isDepthBullet || drawColor.w < 0.999f;
  engine->SetBlendModeForFrame(
      useAlphaBlend ? BlendMode::NORMAL : BlendMode::NONE);
  engine->DrawSphere(
      WhiteTextureIndex(), drawColor, Math::Transform{},
      owner->GetWorldMatrix(), drawRadius, subdivision_,
      isLightingEnabled_);
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum
