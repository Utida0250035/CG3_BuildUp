#pragma once

namespace Atrum {

class Entity;

/// ボス戦闘終了時に残存攻撃オブジェクトを片付ける
void ClearBossAttackTelegraphAndBullets(Entity &boss);

/// 再戦・シーン退出時に戦闘まわりの静的状態をリセットする
void ResetBossCombatSessionState();

} // namespace Atrum
