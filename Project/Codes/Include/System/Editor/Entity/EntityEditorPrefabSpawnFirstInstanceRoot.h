#pragma once

#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace Atrum::EntityEditor {
class EntityEditor;
struct SceneEntityInstance;
}

namespace Atrum::EntityEditor::EntityEditorPrefabSpawnFirstInstanceRoot {

/// プレハブ JSON から先頭 1 体分のインスタンスを組み立てる（親は常にルート）。
std::optional<SceneEntityInstance>
TryBuildRootInstanceFromPrefabJson(const nlohmann::json &json);

/// シーンへ 1 体追加し選択・通知まで行う。失敗時は false。
bool AppendFromPrefabFile(EntityEditor &editor, const std::string &prefabPath);

} // namespace Atrum::EntityEditor::EntityEditorPrefabSpawnFirstInstanceRoot