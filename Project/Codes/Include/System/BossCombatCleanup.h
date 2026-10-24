#pragma once

namespace Atrum {

class Entity;

/// ボス戦闘終了時に残存攻撃オブジェクトを片付ける
void ClearBossAttackTelegraphAndBullets(Entity &boss);

} // namespace Atrum
