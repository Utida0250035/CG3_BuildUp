#include "Scene/ScreenHeartIconDraw.h"

#include "Engine/AtrumEngine.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "System/WhiteTexture.h"

#include <cmath>
#include <numbers>

namespace Atrum::ScreenHeartIcon {

namespace {

/// 中心位置・Z 回転を指定してスプライト矩形を描画
void DrawScreenRectRotated(AtrumEngine *engine, const Math::Vector4 &color,
                           const float centerX, const float centerY,
                           const float width, const float height,
                           const float rotateZ) {
  const float halfW = width * 0.5f;
  const float halfH = height * 0.5f;
  const float cosZ = std::cos(rotateZ);
  const float sinZ = std::sin(rotateZ);
  const float rotatedCenterX = halfW * cosZ - halfH * sinZ;
  const float rotatedCenterY = halfW * sinZ + halfH * cosZ;

  Math::Transform uvTransform{};
  Math::Transform rectTransform{};
  rectTransform.translate = {centerX - rotatedCenterX, centerY - rotatedCenterY,
                             0.0f};
  rectTransform.scale = {width, height, 1.0f};
  rectTransform.rotate = {0.0f, 0.0f, rotateZ};

  engine->DrawSpriteRect(WhiteTextureIndex(), color, uvTransform, rectTransform,
                         Math::Vector2{1.0f, 1.0f}, Math::Vector2{0.0f, 0.0f});
}

} // namespace

/// HP 1 単位分：45° 回転した正方形 3 つ（左右の膨らみ＋下の先端）でハート型に見せる
void Draw(AtrumEngine *engine, const Math::Vector4 &color, const float originX,
          const float originY, const float scale) {
  constexpr float kLobeSize = 12.0f;
  constexpr float kBottomSize = 11.0f;
  constexpr float kLobeRotateZ = std::numbers::pi_v<float> * 0.25f;

  DrawScreenRectRotated(engine, color, originX + 8.0f * scale,
                        originY + 10.0f * scale, kLobeSize * scale,
                        kLobeSize * scale, kLobeRotateZ);
  DrawScreenRectRotated(engine, color, originX + 20.0f * scale,
                        originY + 10.0f * scale, kLobeSize * scale,
                        kLobeSize * scale, kLobeRotateZ);
  DrawScreenRectRotated(engine, color, originX + kSlotSize * 0.5f * scale,
                        originY + 19.0f * scale, kBottomSize * scale,
                        kBottomSize * scale, kLobeRotateZ);
}

} // namespace Atrum::ScreenHeartIcon
