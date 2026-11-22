#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 自機エイムカプセル（半径b・伸長中・エイム入力中）が敵に重なっているか。
bool IsEnemyTargetedByPlayerAimCapsule(const Entity &enemy, Entity &player);

} // namespace Atrum::AtomF