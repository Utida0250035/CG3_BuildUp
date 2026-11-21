#pragma once

namespace Atrum::EntityEditor {

struct ComponentSlot;

/// 分割ボール系 Upd コンポーネントのインスペクタ。登録表とプレビュー経路と同期。
/// 対象型なら true（outChanged に編集有無）。非対象は false。
bool TryDrawBallSplitComponentParams(ComponentSlot &slot, bool &outChanged);

} // namespace Atrum::EntityEditor