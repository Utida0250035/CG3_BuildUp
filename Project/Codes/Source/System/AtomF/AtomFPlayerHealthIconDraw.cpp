#include "System/AtomF/AtomFPlayerHealthIconDraw.h"

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Scene/ScreenHeartIconDraw.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"

#include <algorithm>

namespace Atrum::AtomF {

namespace {

constexpr float kIconScale = 0.6f;
constexpr float kIconGapPixels = 3.0f;
/// 自機球の上端からアイコン下端までの余白（ピクセル）
constexpr float kAboveBodyMarginPixels = 6.0f;

} // namespace

void DrawPlayerHealthIconsAbovePlayer(AtrumEngine *engine, const Camera &camera,
                                      const float viewWidth,
                                      const float viewHeight) {
  if (engine == nullptr || viewWidth <= 0.0f || viewHeight <= 0.0f) {
    return;
  }

  Entity *player = ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return;
  }
  const CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>();
  if (health == nullptr) {
    return;
  }
  const uint32_t healthMax = health->GetInitialHealthCount();
  if (healthMax == 0) {
    return;
  }
  const uint32_t healthNow = std::min(health->GetHealthCount(), healthMax);

  // 自機球の真上の点をスクリーンへ投影する（中央揃えの基準 x と、アイコン下端 y）。
  const Math::Vector3 playerPos = player->GetWorldPosition();
  const Math::Vector3 topPoint{playerPos.x,
                               playerPos.y + ResolvePlayerBodyRadiusMeters(),
                               playerPos.z};
  const Math::Matrix4x4 viewProjection = camera.GetViewProjectionMatrix();
  const Math::Vector3 ndc = viewProjection.Transform(topPoint);
  const float anchorX = (ndc.x * 0.5f + 0.5f) * viewWidth;
  const float anchorTopY = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewHeight;

  const float iconSize = ScreenHeartIcon::kSlotSize * kIconScale;
  const float totalWidth = static_cast<float>(healthMax) * iconSize +
                           static_cast<float>(healthMax - 1U) * kIconGapPixels;
  const float left = anchorX - totalWidth * 0.5f;
  const float top = anchorTopY - kAboveBodyMarginPixels - iconSize;

  const Math::Vector4 aliveColor{0.35f, 0.9f, 1.0f, 1.0f};
  const Math::Vector4 lostColor{0.2f, 0.2f, 0.25f, 0.6f};

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();
  for (uint32_t i = 0; i < healthMax; ++i) {
    const float x = left + static_cast<float>(i) * (iconSize + kIconGapPixels);
    ScreenHeartIcon::Draw(engine, i < healthNow ? aliveColor : lostColor, x,
                          top, kIconScale);
  }
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum::AtomF
