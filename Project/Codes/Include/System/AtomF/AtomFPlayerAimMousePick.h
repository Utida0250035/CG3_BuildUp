#pragma once

namespace Atrum {

class Entity;

namespace AtomF {

/// 画面中心→カーソルの正規化向き（Y を照準 Z に写像）を XZ 照準角（Y 軸回りラジアン）にする。入力が無効なら false。
bool TryResolveAimAngleFromMouse(const Entity &owner, float &outAngleRadians);

} // namespace AtomF

} // namespace Atrum