#pragma once

namespace Atrum::EntityEditor {

struct ComponentSlot;

/// 既知コンポーネントで params が空または欠けているキーを DefaultParams で補う。
void NormalizeKnownComponentSlotParams(ComponentSlot &slot);

} // namespace Atrum::EntityEditor