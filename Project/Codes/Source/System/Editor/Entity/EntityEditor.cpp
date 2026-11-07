#include "System/Editor/Entity/EntityEditor.h"

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/Components/CmpDrawHitSphere.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpRotator.h"
#include "System/Entity/Components/CmpScroll.h"
#include "System/Entity/Entity.h"
#include "System/Json/JsonHandler.h"
#include <filesystem>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

namespace {

void ReadHitSpheresFromJson(const nlohmann::json &j,
                            std::vector<HitSphere> &out) {
  out.clear();
  if (!j.is_array()) {
    return;
  }
  for (const nlohmann::json &item : j) {
    HitSphere sphere{};
    sphere.localOffset =
        item.value("localOffset", Math::Vector3::Zero());
    sphere.radius = item.value("radius", 1.0f);
    out.push_back(sphere);
  }
}

bool TryAddEmptyComponent(Entity &entity, const ComponentSlot &slot) {
  if (slot.typeName == "CmpMeshSphere" && slot.kind == ComponentSlotKind::Draw) {
    entity.AddDrawCmp<CmpMeshSphere>();
    return true;
  }
  if (slot.typeName == "CmpDrawHitSphere" &&
      slot.kind == ComponentSlotKind::Draw) {
    entity.AddDrawCmp<CmpDrawHitSphere>();
    return true;
  }
  if (slot.typeName == "CmpRotator" && slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpRotator>();
    return true;
  }
  if (slot.typeName == "CmpScroll" && slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpScroll>();
    return true;
  }
  if (slot.typeName == "CmpHitSphere" && slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpHitSphere>();
    return true;
  }
  return false;
}

void ApplyComponentParams(Entity &entity, const ComponentSlot &slot) {
  const nlohmann::json &p = slot.params;

  if (slot.typeName == "CmpMeshSphere") {
    CmpMeshSphere *cmp = entity.GetDrawCmp<CmpMeshSphere>();
    cmp->SetRadius(p.value("radius", 1.0f));
    cmp->SetSubdivision(p.value("subdivision", 8u));
    cmp->SetLightingEnabled(p.value("isLightingEnabled", true));
    Math::Vector4 color = p.value("color", Math::Vector4::White());
    cmp->SetColor(color);
    return;
  }

  if (slot.typeName == "CmpDrawHitSphere") {
    CmpDrawHitSphere *cmp = entity.GetDrawCmp<CmpDrawHitSphere>();
    Math::Vector4 color =
        p.value("color", Math::Vector4{0.2f, 0.85f, 1.0f, 1.0f});
    cmp->SetColor(color);
    cmp->SetLightingEnabled(p.value("isLightingEnabled", false));
    return;
  }

  if (slot.typeName == "CmpRotator") {
    CmpRotator *cmp = entity.GetUpdCmp<CmpRotator>();
    Math::Vector3 axis = p.value("rotateAxis", Math::Vector3::UpLh());
    cmp->SetRotateAxis(axis);
    cmp->SetRotateSpeed(p.value("rotateSpeed", 0.0f));
    if (p.value("pivotIsOwner", true)) {
      cmp->SetPivot(&entity);
    }
    return;
  }

  if (slot.typeName == "CmpScroll") {
    CmpScroll *cmp = entity.GetUpdCmp<CmpScroll>();
    cmp->SetScrollSpeed(p.value("scrollSpeed", 0.0f));
    return;
  }

  if (slot.typeName == "CmpHitSphere") {
    CmpHitSphere *cmp = entity.GetUpdCmp<CmpHitSphere>();
    std::vector<HitSphere> spheres;
    if (p.contains("spheres")) {
      ReadHitSpheresFromJson(p.at("spheres"), spheres);
    }
    if (!spheres.empty()) {
      cmp->SetSpheres(spheres);
    }
    cmp->SetIsActive(p.value("isActive", true));
  }
}

} // namespace

void EntityEditor::LoadFromJsonPath(const std::string &jsonFilePath) {
  namespace fs = std::filesystem;
  if (!fs::exists(jsonFilePath)) {
    document_ = EntityDefinition{};
    ComponentSlot meshSlot{};
    meshSlot.kind = ComponentSlotKind::Draw;
    meshSlot.typeName = "CmpMeshSphere";
    meshSlot.params = DefaultParamsForType("CmpMeshSphere");
    ComponentSlot rotatorSlot{};
    rotatorSlot.kind = ComponentSlotKind::Upd;
    rotatorSlot.typeName = "CmpRotator";
    rotatorSlot.params = DefaultParamsForType("CmpRotator");
    document_.components = {meshSlot, rotatorSlot};
    return;
  }
  document_ = Json::JsonHandler::LoadFromFile<EntityDefinition>(jsonFilePath);
  ui_.selectedComponentIndex = 0;
}

void EntityEditor::SaveToJsonPath(const std::string &jsonFilePath) const {
  Json::JsonHandler::SaveToFile(jsonFilePath, document_);
}

bool EntityEditor::TickPendingRequests() {
  bool needApply = false;

  if (ui_.requestReloadFromDisk) {
    ui_.requestReloadFromDisk = false;
    LoadFromJsonPath(ui_.jsonFilePath);
    needApply = true;
  }
  if (ui_.requestSaveToDisk) {
    ui_.requestSaveToDisk = false;
    SaveToJsonPath(ui_.jsonFilePath);
  }
  if (ui_.requestApplyPreview) {
    ui_.requestApplyPreview = false;
    needApply = true;
  }

  return needApply;
}

std::unique_ptr<Entity> EntityEditor::BuildPreviewEntity() const {
  auto entity = std::make_unique<Entity>(document_.entityName);
  entity->SetEntityType(document_.entityType);
  entity->SetInitialTransform(document_.initialTransform);

  for (const ComponentSlot &slot : document_.components) {
    if (!TryAddEmptyComponent(*entity, slot)) {
      continue;
    }
    ApplyComponentParams(*entity, slot);
  }

  entity->ResolveDependencies();
  entity->Initialize();
  return entity;
}

} // namespace Atrum::EntityEditor
