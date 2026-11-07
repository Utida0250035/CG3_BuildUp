#pragma once

#include "Math/Transform.h"
#include "System/Entity/EntityType.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Atrum::EntityEditor {

/// コンポーネントの種別（Entity の追加先）
enum class ComponentSlotKind { Upd, Draw, Data };

/// 1 コンポーネント分の編集データ（型名＋初期パラメータ JSON）
struct ComponentSlot {
  ComponentSlotKind kind = ComponentSlotKind::Draw;
  std::string typeName = "CmpMeshSphere";
  nlohmann::json params = nlohmann::json::object();
};

/// エンティティ 1 体分の定義（初期トランスフォーム＋コンポーネント列）
struct EntityDefinition {
  std::string entityName = "entity_editor_preview";
  EntityType entityType = EntityType::NONE;
  Math::TransformLH initialTransform{};
  std::vector<ComponentSlot> components{};
};

/// 登録済みコンポーネント種別の既定パラメータ（JSON）
nlohmann::json DefaultParamsForType(const std::string &typeName);

void to_json(nlohmann::json &j, const ComponentSlot &slot);
void from_json(const nlohmann::json &j, ComponentSlot &slot);
void to_json(nlohmann::json &j, const EntityDefinition &definition);
void from_json(const nlohmann::json &j, EntityDefinition &definition);

} // namespace Atrum::EntityEditor
