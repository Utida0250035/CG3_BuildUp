#pragma once

#include "Math/Vector2.h"

namespace Atrum::AtomF {

/// atomF 自機向け。WASD と左スティックを XZ 用の -1〜1 ベクトルにまとめる。
Math::Vector2 SampleAtomFMoveInputXZ();

} // namespace Atrum::AtomF