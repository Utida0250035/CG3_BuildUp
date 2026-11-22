#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 自機弾に parameters の hitShape が sphere のとき、半径a の CmpHitSphere（球当たり）を付与する。
void AttachPlayerBulletHitSphere(Entity &entity);

/// 当たり（CmpHitSphere）と表示（CmpMeshSphere）を parameters の半径a で揃える。
void SyncPlayerBulletHitRadiusA(Entity &entity);

} // namespace Atrum::AtomF