#include "System/AtomF/AtomFBossHealthBarDraw.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/WhiteTexture.h"

#include <algorithm>
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
  if (engine == nullptr || viewWidth <= 0.0f || viewHeight <= 0.0f) {
    return;
  }
  const CmpAtomFBoss *boss = CmpAtomFBoss::Current();
  if (boss == nullptr) {
    return;
  }

  const float barWidth = viewWidth * 0.6f;
  const float barHeight = std::max(10.0f, viewHeight * 0.022f);
  const float left = (viewWidth - barWidth) * 0.5f;
  const float top = viewHeight * 0.04f;
  const float border = 2.0f;

  const float reveal = boss->HpBarReveal01();
  const float shownWidth = barWidth * reveal;
  const float real = boss->HealthRatio();
  const float trail = boss->DisplayedHealthRatio();

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();

  // 枠と背景（登場中は枠だけ伸びる）。
  DrawRect(engine, Math::Vector4{0.05f, 0.02f, 0.08f, 0.85f}, left - border,
           top - border, shownWidth + border * 2.0f, barHeight + border * 2.0f);

  // 遅れて縮む白い残像 → 実 Hp の順に重ねる。
  DrawRect(engine, Math::Vector4{1.0f, 1.0f, 1.0f, 0.85f}, left, top,
           shownWidth * std::clamp(trail, 0.0f, 1.0f), barHeight);

  // Hp の少なさで紫→赤へ。激昂中は脈動させる。
  const float danger = 1.0f - std::clamp(real, 0.0f, 1.0f);
  Math::Vector4 fill{0.65f + 0.35f * danger, 0.2f * (1.0f - danger), 0.85f - 0.65f * danger, 1.0f};
  if (boss->IsEnraged()) {
    const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(boss->Health()) * 0.9f);
    fill.x = std::min(1.0f, fill.x + 0.15f * pulse);
  }
  DrawRect(engine, fill, left, top, shownWidth * std::clamp(real, 0.0f, 1.0f),
           barHeight);

  // 1/4 ごとの目盛り。
  for (int i = 1; i < 4; ++i) {
    DrawRect(engine, Math::Vector4{0.0f, 0.0f, 0.0f, 0.6f},
             left + shownWidth * static_cast<float>(i) / 4.0f - 1.0f, top, 2.0f,
             barHeight);
  }

  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum::AtomF
