#pragma once

namespace Atrum {

class Entity;

namespace AtomF {

/// 指定した当たり半径で CmpAtomFArenaContainment を付与する（未付なら追加）。
void AttachArenaContainment(Entity &entity, float bodyRadiusMeters);

/// 自機向け（半径b）
void AttachArenaContainmentForPlayer(Entity &entity);

/// 自機弾向け（半径a）
void AttachArenaContainmentForPlayerBullet(Entity &entity);

/// 敵向け
void AttachArenaContainmentForEnemy(Entity &entity);

} // namespace AtomF

} // namespace Atrum