#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <optional>
#include <string>

namespace Atrum::EntityEditor {

/// ヒエラルキー「オブジェクトを追加」用テンプレート
enum class SceneObjectAddPreset {
  CopySelection,
  Empty,
  StarterSample,
  MainCamera,
};

/// プリセットから EntityDefinition を組み立てる（失敗時は nullopt）
std::optional<EntityDefinition>
BuildEntityDefinitionForAddPreset(SceneObjectAddPreset preset,
                                const std::string &entityName);

} // namespace Atrum::EntityEditor