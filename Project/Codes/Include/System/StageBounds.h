#pragma once

namespace Atrum {

// Transplant の剛体が使っていた横画面の端
inline constexpr float kGroundHeight = -4.0f;
inline constexpr float kCeilHeight = 8.0f;
inline constexpr float kLeftEndX = -14.0f;
inline constexpr float kRightEndX = 14.0f;

/// メテオ／壁弾など画面上部から落とす弾の生成 Y（カメラ視野にも含める）
inline constexpr float kBarrageSpawnTopY = kCeilHeight + 1.25f;

} // namespace Atrum
