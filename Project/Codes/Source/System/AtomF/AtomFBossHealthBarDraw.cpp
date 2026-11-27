#include "System/AtomF/AtomFBossHealthBarDraw.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/WhiteTexture.h"

#include <algorithm>

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
  const CmpAtomFBoss *boss = CmpAtomFBoss::Active();
  if (engine == nullptr || boss == nullptr || viewWidth <= 0.0f ||
      viewHeight <= 0.0f) {
    return;
  }
  if (boss->CurrentState() == CmpAtomFBoss::State::Appearing) {
    return;
  }

  const float barWidth = viewWidth * 0.6f;
  const float barHeight = std::max(8.0f, viewHeight * 0.022f);
  const float left = (viewWidth - barWidth) * 0.5f;
  const float top = viewHeight * 0.04f;
  const float frame = 3.0f;

  Math::Vector4 fillColor{0.85f, 0.25f, 0.9f, 1.0f};
  if (boss->IsEnraged()) {
    fillColor = Math::Vector4{1.0f, 0.2f, 0.15f, 1.0f};
  }
  if (boss->CurrentState() == CmpAtomFBoss::State::Stunned) {
    fillColor = Math::Vector4{1.0f, 0.9f, 0.3f, 1.0f}; // 弱点（ダメージ増）の合図
  }

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();
  DrawRect(engine, Math::Vector4{0.05f, 0.05f, 0.08f, 0.85f}, left - frame,
           top - frame, barWidth + frame * 2.0f, barHeight + frame * 2.0f);
  DrawRect(engine, Math::Vector4{0.25f, 0.1f, 0.12f, 1.0f}, left, top, barWidth,
           barHeight);
  DrawRect(engine, fillColor, left, top, barWidth * boss->HpRatio(), barHeight);
  if (!boss->IsEnraged()) {
    // 激昂しきい値の目印。
    DrawRect(engine, Math::Vector4{1.0f, 1.0f, 1.0f, 0.9f},
             left + barWidth * boss->EnrageHpRatio() - 1.0f, top - 2.0f, 2.0f,
             barHeight + 4.0f);
  }
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum::AtomF
