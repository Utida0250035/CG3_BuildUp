#pragma once

#include "System/Entity/Entity.h"

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

/// atomF の雑魚敵を 1 体、変種名（parameters.json の enemy.variants[].name）で指定してスポーンする。
/// 更新ループ中に呼んでよい（採用キュー経由でシーンへ入る）。
void EnqueueAtomFEnemy(const std::string &variantName, float x, float z);

/// atomF のボスをスポーンする（Hp・スキルは parameters.json の boss）。
void EnqueueAtomFBoss(float x, float z);

} // namespace Atrum
