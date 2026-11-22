#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 自機弾に parameters の hitShape が sphere のとき、半径a の CmpHitSphere（球当たり）を付与する。
void AttachPlayerBulletHitSphere(Entity &entity);

} // namespace Atrum::AtomF