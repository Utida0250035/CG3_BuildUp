#include "System/Editor/Entity/EntityEditorDefinition.h"

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

namespace {

NLOHMANN_JSON_SERIALIZE_ENUM(ComponentSlotKind,
                            {{ComponentSlotKind::Upd, "Upd"},
                             {ComponentSlotKind::Draw, "Draw"},
                             {ComponentSlotKind::Data, "Data"}})

EntityType EntityTypeFromString(const std::string &name) {
  if (name == "PLAYER") {
    return EntityType::PLAYER;
  }
  if (name == "ENEMY") {
    return EntityType::ENEMY;
  }
  if (name == "PLAYER_WEAPON") {
    return EntityType::PLAYER_WEAPON;
  }
  if (name == "ENEMY_WEAPON") {
    return EntityType::ENEMY_WEAPON;
  }
  if (name == "PLAYER_ITEM") {
    return EntityType::PLAYER_ITEM;
  }
  if (name == "ENEMY_ITEM") {
    return EntityType::ENEMY_ITEM;
  }
  return EntityType::NONE;
}

const char *EntityTypeToString(const EntityType type) {
  switch (type) {
  case EntityType::PLAYER:
    return "PLAYER";
  case EntityType::ENEMY:
    return "ENEMY";
  case EntityType::PLAYER_WEAPON:
    return "PLAYER_WEAPON";
  case EntityType::ENEMY_WEAPON:
    return "ENEMY_WEAPON";
  case EntityType::PLAYER_ITEM:
    return "PLAYER_ITEM";
  case EntityType::ENEMY_ITEM:
    return "ENEMY_ITEM";
  case EntityType::NONE:
  default:
    return "NONE";
  }
}

} // namespace

nlohmann::json DefaultParamsForType(const std::string &typeName) {
  if (typeName == "CmpMeshSphere") {
    return {{"radius", 1.0f},
            {"subdivision", 12},
            {"isLightingEnabled", true},
            {"color", Math::Vector4::White()}};
  }
  if (typeName == "CmpRotator") {
    return {{"rotateAxis", Math::Vector3::UpLh()},
            {"rotateSpeed", 1.5f},
            {"pivotIsOwner", true}};
  }
  if (typeName == "CmpScroll") {
    return {{"scrollSpeed", 2.0f}};
  }
  if (typeName == "CmpHitSphere") {
    return nlohmann::json{{"isActive", true},
                          {"spheres", nlohmann::json::array({nlohmann::json{
                                          {"localOffset", Math::Vector3::Zero()},
                                          {"radius", 0.5f}}})}};
  }
  if (typeName == "CmpDrawHitSphere") {
    return {{"color", Math::Vector4{0.2f, 0.85f, 1.0f, 1.0f}},
            {"isLightingEnabled", false}};
  }
  return nlohmann::json::object();
}

void to_json(nlohmann::json &j, const ComponentSlot &slot) {
  j = nlohmann::json{{"kind", slot.kind},
                     {"typeName", slot.typeName},
                     {"params", slot.params}};
}

void from_json(const nlohmann::json &j, ComponentSlot &slot) {
  if (j.contains("kind") && !j.at("kind").is_null()) {
    from_json(j.at("kind"), slot.kind);
  }
  slot.typeName = j.value("typeName", slot.typeName);
  slot.params = j.value("params", nlohmann::json::object());
}

void to_json(nlohmann::json &j, const EntityDefinition &definition) {
  j = nlohmann::json{{"entityName", definition.entityName},
                     {"entityType", EntityTypeToString(definition.entityType)},
                     {"initialTransform", definition.initialTransform},
                     {"components", definition.components}};
}

void from_json(const nlohmann::json &j, EntityDefinition &definition) {
  definition.entityName = j.value("entityName", definition.entityName);
  if (j.contains("entityType")) {
    definition.entityType =
        EntityTypeFromString(j.at("entityType").get<std::string>());
  }
  definition.initialTransform =
      j.value("initialTransform", definition.initialTransform);
  definition.components = j.value("components", definition.components);
}

} // namespace Atrum::EntityEditor
