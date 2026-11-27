#pragma once

#include <string>

namespace Atrum {

/// atomF の雑魚を variantName の種類で (x, z) に出す。名前が空・未定義なら先頭の種類。
/// 更新中でも安全（採用待ち行列経由。次の走査後にシーンへ入る）。出現演出つき。
void EnqueueAtomFEnemy(const std::string &variantName, float x, float z);

/// atomF のボスを (x, z) に出す。すでに居る場合は何もしない。出現演出つき。
void EnqueueAtomFBoss(float x, float z);

} // namespace Atrum
