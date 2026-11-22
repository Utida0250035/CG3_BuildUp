#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 敵に球当たり（押し出し半径と同一のフォールバック）を付与する。
void AttachEnemyHitSphere(Entity &entity);

} // namespace Atrum::AtomF