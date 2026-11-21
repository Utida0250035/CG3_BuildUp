#pragma once

namespace Atrum::EntityEditor {

class EntityEditor;

/// 選択中（複数可）インスタンスを複製する（定義・コンポーネントをコピー）。
bool DuplicateSelectedInstanceInScene(EntityEditor &editor);

} // namespace Atrum::EntityEditor