#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 自機に半径b（parameters の hitRadiusMeters）の球当たりを付与する。未付なら CmpHitSphere を追加する。
void AttachPlayerHitSphere(Entity &entity);

} // namespace Atrum::AtomF