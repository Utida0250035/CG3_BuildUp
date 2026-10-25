#pragma once

#include "Math/Lerp.h"
#include "Math/Vector4.h"
#include "System/Entity/EntityType.h"
#include "System/StageBounds.h"

#include <algorithm>

namespace Atrum::BulletDepthVisual {

/// 2D 戦闘（XY 平面）向けの見た目用奥行き。0=画面下寄り=手前, 1=画面上寄り=奥
inline float PseudoDepthLane01FromWorldY(const float worldY) {
  const float span = kBarrageSpawnTopY - kGroundHeight;
  const float lane = (worldY - kGroundHeight) / std::max(span, 1.0e-4f);
  return std::clamp(lane, 0.0f, 1.0f);
}

inline bool IsManagedBulletEntityType(const EntityType type) {
  return type == EntityType::ENEMY_WEAPON || type == EntityType::PLAYER_ITEM;
}

/// 描画半径用（当たり判定・座標 Z は変えない）
inline float DrawScaleFromPseudoDepth(const float pseudoDepthLane01) {
  return Math::Lerp(1.14f, 0.82f, pseudoDepthLane01);
}

inline Math::Vector4 ModulateBulletColor(const Math::Vector4 &baseColor,
                                         const float pseudoDepthLane01,
                                         const EntityType type) {
  if (!IsManagedBulletEntityType(type)) {
    return baseColor;
  }

  const float brightness = Math::Lerp(1.1f, 0.72f, pseudoDepthLane01);

  Math::Vector4 color = baseColor;
  color.x *= brightness;
  color.y *= brightness;
  color.z *= brightness;

  const float cool = pseudoDepthLane01 * 0.06f;
  const float warm = (1.0f - pseudoDepthLane01) * 0.05f;
  color.x += warm;
  color.z += cool;

  color.x = std::clamp(color.x, 0.0f, 1.5f);
  color.y = std::clamp(color.y, 0.0f, 1.5f);
  color.z = std::clamp(color.z, 0.0f, 1.5f);
  return color;
}

inline float DrawScaleFromBulletWorldY(const float worldY) {
  return DrawScaleFromPseudoDepth(PseudoDepthLane01FromWorldY(worldY));
}

inline Math::Vector4 ModulateBulletColorAtWorldY(const Math::Vector4 &baseColor,
                                                 const float worldY,
                                                 const EntityType type) {
  return ModulateBulletColor(baseColor, PseudoDepthLane01FromWorldY(worldY),
                             type);
}

/// 奥行きに応じた本体のアルファ（奥ほど薄くして距離感を出す）
inline float BodyAlphaFromPseudoDepth(const float pseudoDepthLane01) {
  return Math::Lerp(1.0f, 0.88f, pseudoDepthLane01);
}

/// 地面に落とす楕円影（描画のみ。前景雲の上端より上に置く）
inline float GroundShadowRadiusScale(const float pseudoDepthLane01) {
  return Math::Lerp(1.22f, 0.68f, pseudoDepthLane01);
}

inline float GroundShadowVerticalSquash() { return 0.14f; }

inline Math::Vector4 GroundShadowColor(const float pseudoDepthLane01) {
  const float alpha = Math::Lerp(0.42f, 0.14f, pseudoDepthLane01);
  return Math::Vector4{0.02f, 0.02f, 0.05f, alpha};
}

/// 加算グロー（手前ほど強め）
inline float GlowRadiusScale(const float pseudoDepthLane01) {
  return Math::Lerp(1.38f, 1.12f, pseudoDepthLane01);
}

inline Math::Vector4 GlowColorFromBody(const Math::Vector4 &bodyColor,
                                       const float pseudoDepthLane01) {
  const float strength = Math::Lerp(0.55f, 0.28f, pseudoDepthLane01);
  Math::Vector4 glow = bodyColor;
  glow.x = std::min(glow.x * strength + 0.08f, 1.5f);
  glow.y = std::min(glow.y * strength + 0.06f, 1.5f);
  glow.z = std::min(glow.z * strength + 0.1f, 1.5f);
  glow.w = Math::Lerp(0.38f, 0.18f, pseudoDepthLane01) * bodyColor.w;
  return glow;
}

/// EntityFactory::ForegroundGroundClouds 接地帯の上端（y≈-5.5 + radius≈1.17）
inline constexpr float kForegroundGroundCloudApexY = -4.35f;
/// 影位置計算用の最大描画半径（DrawScale 込みの余裕）
inline constexpr float kGroundShadowMaxRadiusForLayout = 0.75f;
inline constexpr float kGroundShadowClearanceAboveCloudApexY = 0.07f;

inline float GroundShadowMaxHalfHeightWorldY() {
  return kGroundShadowMaxRadiusForLayout * GroundShadowRadiusScale(0.0f) *
         GroundShadowVerticalSquash();
}

inline float GroundShadowCenterWorldY() {
  return kForegroundGroundCloudApexY + GroundShadowMaxHalfHeightWorldY() +
         kGroundShadowClearanceAboveCloudApexY;
}

} // namespace Atrum::BulletDepthVisual
