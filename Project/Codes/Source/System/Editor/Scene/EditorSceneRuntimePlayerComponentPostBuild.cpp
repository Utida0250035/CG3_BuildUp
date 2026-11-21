#include "System/Editor/Scene/EditorSceneRuntimePlayerComponentPostBuild.h"

#include "System/Entity/Components/CmpParry.h"
#include "System/Entity/Components/CmpSwing.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/Playable/CmpMoveByInput.h"

#include <unordered_map>
#include <string>

namespace Atrum::EntityEditor {

namespace {

void BuildInstanceNameMap(
    const EditorSceneDocument &document,
    const std::vector<std::unique_ptr<Entity>> &entities,
    std::unordered_map<std::string, Entity *> &outMap) {
  const size_t count =
      std::min(document.instances.size(), entities.size());
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

} // namespace

void ApplyPlayerComponentPostBuild(
    const EditorSceneDocument &document,
    std::vector<std::unique_ptr<Entity>> &entities) {
  std::unordered_map<std::string, Entity *> instanceMap{};
  BuildInstanceNameMap(document, entities, instanceMap);

  const size_t count =
      std::min(document.instances.size(), entities.size());
  for (size_t i = 0; i < count; ++i) {
    Entity *entity = entities[i].get();
    if (entity == nullptr) {
      continue;
    }

    if (entity->HasUpdCmp<CmpSwing>()) {
      CmpSwing *swing = entity->GetUpdCmp<CmpSwing>();
      if (swing->GetPivot() == nullptr &&
          !swing->PivotInstanceName().empty()) {
        if (Entity *pivot =
                FindByInstanceName(instanceMap, swing->PivotInstanceName())) {
          swing->SetPivot(pivot);
        }
      }
    }

    if (entity->HasDataCmp<CmpParry>()) {
      CmpParry *cmpParry = entity->GetDataCmp<CmpParry>();
      bool autoCreate = true;
      for (const ComponentSlot &slot :
           document.instances[i].definition.components) {
        if (slot.typeName == "CmpParry") {
          autoCreate = slot.params.value("autoCreateParryWeapon", true);
          break;
        }
      }
      if (autoCreate && !cmpParry->HasEntParry()) {
        std::unique_ptr<Entity> parryWeapon = EntityFactory::Parry(entity);
        cmpParry->SetEntParry(parryWeapon.release());
      }
    }

    if (entity->HasUpdCmp<Player::CmpMoveByInput2D>()) {
      Player::CmpMoveByInput2D *move =
          entity->GetUpdCmp<Player::CmpMoveByInput2D>();
      std::string forwardName{};
      std::string rearName{};
      float sideOffset = 0.65f;
      for (const ComponentSlot &slot :
           document.instances[i].definition.components) {
        if (slot.typeName != "CmpMoveByInput2D") {
          continue;
        }
        forwardName =
            slot.params.value("directionMarkerForwardInstance", std::string{});
        rearName =
            slot.params.value("directionMarkerRearInstance", std::string{});
        sideOffset = slot.params.value("directionMarkerSideOffset", 0.65f);
        break;
      }
      Entity *forward =
          forwardName.empty() ? nullptr
                              : FindByInstanceName(instanceMap, forwardName);
      Entity *rear =
          rearName.empty() ? nullptr
                           : FindByInstanceName(instanceMap, rearName);
      if (forward != nullptr && rear != nullptr) {
        move->SetDirectionMarkers(forward, rear, sideOffset);
      }
    }
  }
}

} // namespace Atrum::EntityEditor