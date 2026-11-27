#include "System/AtomF/AtomFBossHealthBarDraw.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/WhiteTexture.h"

#include <cmath>

namespace Atrum::AtomF {

namespace {

void DrawRect(AtrumEngine *engine, const Math::Vector4 &color, const float x,
              const float y, const float width, const float height) {
  if (width <= 0.0f || height <= 0.0f) {
    return;
  }
  Math::Transform uvTransform{};
  Math::Transform rectTransform{};
  rectTransform.translate = {x, y, 0.0f};
  rectTransform.scale = {width, height, 1.0f};
  engine->DrawSpriteRect(WhiteTextureIndex(), color, uvTransform, rectTransform,
                         Math::Vector2{1.0f, 1.0f}, Math::Vector2{0.0f, 0.0f});
}

} // namespace

void DrawBossHealthBar(AtrumEngine *engine, const float viewWidth,
                       const float viewHeight) {
  CmpAtomFBoss *boss = CmpAtomFBoss::Active();
  if (engine == nullptr || boss == nullptr || viewWidth <= 0.0f ||
      viewHeight <= 0.0f) {
    return;
  }

  const float barWidth = viewWidth * 0.5f;
  const float barHeight = 14.0f;
  const float left = (viewWidth - barWidth) * 0.5f;
  const float top = viewHeight - 44.0f;
  const float border = 2.0f;

  const float fill = boss->HealthBar01();
  const Math::Vector4 fillColor =
      boss->IsStunned() ? Math::Vector4{0.35f, 0.75f, 1.0f, 1.0f}
      : boss->IsPhase2() ? Math::Vector4{0.95f, 0.2f, 0.2f, 1.0f}
                         : Math::Vector4{0.75f, 0.3f, 0.95f, 1.0f};

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();
  DrawRect(engine, Math::Vector4{0.0f, 0.0f, 0.0f, 0.65f}, left - border,
           top - border, barWidth + border * 2.0f, barHeight + border * 2.0f);
  DrawRect(engine, Math::Vector4{0.18f, 0.12f, 0.22f, 0.9f}, left, top,
           barWidth, barHeight);
  DrawRect(engine, fillColor, left, top, barWidth * fill, barHeight);
  // 第 2 形態に入る Hp 位置の目印。
  const float markX =
      left + barWidth * ResolveBossParameters().phase2HealthRatio;
  DrawRect(engine, Math::Vector4{1.0f, 1.0f, 1.0f, 0.8f}, markX - 1.0f,
           top - 3.0f, 2.0f, barHeight + 6.0f);
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum::AtomF
