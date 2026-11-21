#include "System/Editor/Scene/EditorSceneRuntimePlayerComponentPostBuild.h"

#include "System/Entity/Components/CmpSwing.h"
#include "System/Entity/Entity.h"
#include "System/Playable/CmpMoveByInput.h"
#include "System/Playable/PlayerParryRuntimeAssembly.h"

#include <string>
#include <unordered_map>

namespace Atrum::EntityEditor {

namespace {

void BuildInstanceNameMap(
    const EditorSceneDocument &document,
    const std::vector<std::unique_ptr<Entity>> &entities,
    std::unordered_map<std::string, Entity *> &outMap) {
  const size_t count = std::min(document.instances.size(), entities.size());
  for (size_t i = 0; i < count; ++i) {
    if (!entities[i]) {
      continue;
    }
    const std::string &name = document.instances[i].instanceName;
    if (!name.empty()) {
      outMap[name] = entities[i].get();
    }
  }
}

Entity *FindByInstanceName(
    const std::unordered_map<std::string, Entity *> &map,
    const std::string &instanceName) {
  const auto it = map.find(instanceName);
  if (it == map.end()) {
    return nullptr;
  }
  return it->second;
}

Player::PlayerParryRuntimeAssemblyOptions
OptionsFromDefinition(const EntityDefinition &definition) {
  Player::PlayerParryRuntimeAssemblyOptions options{};
  for (const ComponentSlot &slot : definition.components) {
    if (slot.typeName == "CmpParry") {
      options.createParryWeapon =
          slot.params.value("autoCreateParryWeapon", true);
    }
    if (slot.typeName == "CmpMoveByInput2D") {
      options.createDirectionMarkersIfMissing =
          slot.params.value("useDefaultPlayerParryAssembly", true);
      options.directionMarkerSideOffset =
          slot.params.value("directionMarkerSideOffset", 0.65f);

      const std::string forwardName = slot.params.value(
          "directionMarkerForwardInstance", std::string{});
      const std::string rearName = slot.params.value(
          "directionMarkerRearInstance", std::string{});
      if (!forwardName.empty() || !rearName.empty()) {
        options.createDirectionMarkersIfMissing = false;
      }
    }
  }
  return options;
}

void ResolveNamedPlayerLinks(
    const EntityDefinition &definition, Entity &entity,
    const std::unordered_map<std::string, Entity *> &instanceMap) {
  if (entity.HasUpdCmp<CmpSwing>()) {
    CmpSwing *swing = entity.GetUpdCmp<CmpSwing>();
    if (swing->GetPivot() == nullptr && !swing->PivotInstanceName().empty()) {
      if (Entity *pivot =
              FindByInstanceName(instanceMap, swing->PivotInstanceName())) {
        swing->SetPivot(pivot);
      }
    }
  }

  if (!entity.HasUpdCmp<Player::CmpMoveByInput2D>()) {
    return;
  }
  Player::CmpMoveByInput2D *move = entity.GetUpdCmp<Player::CmpMoveByInput2D>();

  std::string forwardName{};
  std::string rearName{};
  float sideOffset = 0.65f;
  for (const ComponentSlot &slot : definition.components) {
    if (slot.typeName != "CmpMoveByInput2D") {
      continue;
    }
    forwardName =
        slot.params.value("directionMarkerForwardInstance", std::string{});
    rearName = slot.params.value("directionMarkerRearInstance", std::string{});
    sideOffset = slot.params.value("directionMarkerSideOffset", 0.65f);
    break;
  }

  Entity *forward =
      forwardName.empty() ? nullptr : FindByInstanceName(instanceMap, forwardName);
  Entity *rear =
      rearName.empty() ? nullptr : FindByInstanceName(instanceMap, rearName);
  if (forward != nullptr && rear != nullptr) {
    move->SetDirectionMarkers(forward, rear, sideOffset);
  }
}

} // namespace

void ApplyPlayerComponentPostBuild(
    const EditorSceneDocument &document,
    std::vector<std::unique_ptr<Entity>> &entities) {
  std::unordered_map<std::string, Entity *> instanceMap{};
  BuildInstanceNameMap(document, entities, instanceMap);

  const size_t count = std::min(document.instances.size(), entities.size());
  for (size_t i = 0; i < count; ++i) {
    Entity *entity = entities[i].get();
    if (entity == nullptr) {
      continue;
    }
    if (!Player::EntityQualifiesForPlayerParryAssembly(*entity)) {
      continue;
    }

    ResolveNamedPlayerLinks(document.instances[i].definition, *entity,
                            instanceMap);

    Player::PlayerParryRuntimeAssemblyOptions options =
        OptionsFromDefinition(document.instances[i].definition);
    Player::AssemblePlayerParryRuntimeExtras(*entity, options);
  }
}

} // namespace Atrum::EntityEditor