#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 自機弾に半径a（parameters）の球当たりを付与する。
void AttachPlayerBulletHitSphere(Entity &entity);

} // namespace Atrum::AtomF