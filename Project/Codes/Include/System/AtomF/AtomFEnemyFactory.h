#pragma once

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersRuntime.h"

#include <memory>
#include <string>

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// parameters の enemy.variants から名前で探す。無ければ nullptr。
const EnemyVariantParameters *FindEnemyVariantByName(const std::string &name);

/// 雑魚敵 1 体を組み立てる（ResolveDependencies / Initialize 済み）。
/// groundPosition は XZ のみ使い、高さは本体半径に合わせる。全種 Hp1・耐久系能力なし。
std::unique_ptr<Entity> CreateEnemyEntity(const EnemyVariantParameters &variant,
                                          const Math::Vector3 &groundPosition,
                                          int serialNumber);

} // namespace Atrum::AtomF
