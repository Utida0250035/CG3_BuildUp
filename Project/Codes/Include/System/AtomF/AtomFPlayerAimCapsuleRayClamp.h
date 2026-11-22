#pragma once

#include "Math/Vector3.h"

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// エイムカプセル軸の最大長（壁）。衝突なしなら false。
bool TryMaxAimCapsuleLengthToArenaWall(const Math::Vector3 &startWorld,
                                       const Math::Vector3 &dirUnitXZ,
                                       const float capsuleRadiusMeters,
                                       float &outMaxLengthMeters);

/// 自機以外の敵 Hit との最早衝突距離。見つからなければ false。
bool TryMaxAimCapsuleLengthToEnemies(const Entity &player,
                                     const Math::Vector3 &startWorld,
                                     const Math::Vector3 &dirUnitXZ,
                                     const float capsuleRadiusMeters,
                                     const float searchLengthMeters,
                                     float &outMaxLengthMeters);

/// 壁と敵を踏まえたエイムカプセルの長さ上限。
float ResolveAimCapsuleClampedMaxLength(const Entity &player,
                                        const Math::Vector3 &startWorld,
                                        const Math::Vector3 &dirUnitXZ,
                                        const float capsuleRadiusMeters,
                                        const float nominalMaxLengthMeters);

} // namespace Atrum::AtomF