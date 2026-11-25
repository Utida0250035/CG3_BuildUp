#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 敵ヒットや壁ヒット後、自機弾を無効化し軌道残留があれば linger へ遷移する。
void ConsumePlayerBulletAfterHit(Entity &bullet);

} // namespace Atrum::AtomF