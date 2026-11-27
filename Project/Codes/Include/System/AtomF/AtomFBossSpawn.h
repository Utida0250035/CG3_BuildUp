#pragma once

#include "Math/Vector3.h"

namespace Atrum::AtomF {

/// ボスを生成し、更新ループ後にシーンへ採用されるよう待ち行列へ積む（更新中に呼んでよい）。
void SpawnBoss(const Math::Vector3 &groundPosition);

} // namespace Atrum::AtomF
