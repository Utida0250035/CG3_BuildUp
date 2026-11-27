#pragma once

#include "System/Entity/Entity.h"

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

/// atomF シーン入場時の敵配置（現在は何も置かない。出現はステージスクリプトが行う）。
void AppendAtomFEnemyIfScene(const std::string &sceneJsonPath,
                             std::vector<std::unique_ptr<Entity>> &entities);

namespace AtomF {

/// parameters の enemy.variants の件数（未読込・未定義なら 1）。
int EnemyVariantCount();

/// variants の name から添字を引く。無ければ -1。
int FindEnemyVariantIndex(const std::string &name);

/// 雑魚を 1 体出す。更新走査中でも安全（待ち行列経由でシーンへ採用される）。
/// variantIndex は件数で剰余を取る。
bool SpawnEnemyAtWorld(int variantIndex, float worldX, float worldZ);

/// ボスを 1 体出す（出現演出つき）。更新走査中でも安全。
bool SpawnBossAtWorld(float worldX, float worldZ);

} // namespace AtomF

} // namespace Atrum