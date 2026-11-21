#pragma once

namespace Atrum::EntityEditor {

class EntityEditor;

/// 選択中インスタンスを同一親の兄弟として複製する（定義・コンポーネントをコピー）。
bool DuplicateSelectedInstanceInScene(EntityEditor &editor);

} // namespace Atrum::EntityEditor