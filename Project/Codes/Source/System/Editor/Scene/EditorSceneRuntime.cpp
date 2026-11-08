#include "System/Editor/Scene/EditorSceneRuntime.h"

#include "Engine/AtrumEngine.h"
#include "System/Entity/Components/CmpDrawHitSphere.h"
#include "System/Entity/Components/CmpEditorAssetPath.h"
#include "System/Entity/Components/CmpHitSphere.h" // HitSphere
#include "System/Entity/Components/CmpEditorTextureBillboard.h"
#include "System/Entity/Components/CmpMesh.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpRotator.h"
#include "System/Entity/Components/CmpScroll.h"
#include "System/Entity/Entity.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <cctype>
#include <memory>
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

bool EntityHasComponentForSlot(const Entity &entity, const ComponentSlot &slot) {
  if (slot.kind == ComponentSlotKind::Draw) {
    if (slot.typeName == "CmpMeshSphere") {
      return entity.HasDrawCmp<CmpMeshSphere>();
    }
    if (slot.typeName == "CmpDrawHitSphere") {
      return entity.HasDrawCmp<CmpDrawHitSphere>();
    }
  }
  if (slot.kind == ComponentSlotKind::Upd) {
    if (slot.typeName == "CmpRotator") {
      return entity.HasUpdCmp<CmpRotator>();
    }
    if (slot.typeName == "CmpScroll") {
      return entity.HasUpdCmp<CmpScroll>();
    }
    if (slot.typeName == "CmpHitSphere") {
      return entity.HasUpdCmp<CmpHitSphere>();
    }
  }
  if (slot.kind == ComponentSlotKind::Data) {
    if (slot.typeName == "CmpEditorAssetPath") {
      return entity.HasDataCmp<CmpEditorAssetPath>();
    }
  }
  return false;
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
  if (slot.typeName == "CmpEditorAssetPath" &&
      slot.kind == ComponentSlotKind::Data) {
    entity.AddDataCmp<CmpEditorAssetPath>();
    return true;
  }
  return false;
}

void ApplySlotExecutionOrder(Entity &entity, const ComponentSlot &slot) {
  if (slot.executionOrder <= 0) {
    return;
  }
  const uint64_t priority = static_cast<uint64_t>(slot.executionOrder);

  if (slot.kind == ComponentSlotKind::Upd) {
    if (slot.typeName == "CmpRotator" && entity.HasUpdCmp<CmpRotator>()) {
      entity.SetUpdCmpPriority(entity.GetUpdCmp<CmpRotator>(), priority);
    } else if (slot.typeName == "CmpScroll" && entity.HasUpdCmp<CmpScroll>()) {
      entity.SetUpdCmpPriority(entity.GetUpdCmp<CmpScroll>(), priority);
    } else if (slot.typeName == "CmpHitSphere" &&
               entity.HasUpdCmp<CmpHitSphere>()) {
      entity.SetUpdCmpPriority(entity.GetUpdCmp<CmpHitSphere>(), priority);
    }
    return;
  }

  if (slot.kind == ComponentSlotKind::Draw) {
    if (slot.typeName == "CmpMeshSphere" && entity.HasDrawCmp<CmpMeshSphere>()) {
      entity.SetDrawCmpPriority(entity.GetDrawCmp<CmpMeshSphere>(), priority);
    } else if (slot.typeName == "CmpDrawHitSphere" &&
               entity.HasDrawCmp<CmpDrawHitSphere>()) {
      entity.SetDrawCmpPriority(entity.GetDrawCmp<CmpDrawHitSphere>(), priority);
    }
  }
}

void ApplyComponentParams(Entity &entity, const ComponentSlot &slot) {
  const nlohmann::json &p = slot.params;

  if (slot.typeName == "CmpMeshSphere") {
    if (!entity.HasDrawCmp<CmpMeshSphere>()) {
      return;
    }
    CmpMeshSphere *cmp = entity.GetDrawCmp<CmpMeshSphere>();
    cmp->SetRadius(p.value("radius", 1.0f));
    cmp->SetSubdivision(p.value("subdivision", 8u));
    cmp->SetLightingEnabled(p.value("isLightingEnabled", true));
    Math::Vector4 color = p.value("color", Math::Vector4::White());
    cmp->SetColor(color);
    return;
  }

  if (slot.typeName == "CmpDrawHitSphere") {
    if (!entity.HasDrawCmp<CmpDrawHitSphere>()) {
      return;
    }
    CmpDrawHitSphere *cmp = entity.GetDrawCmp<CmpDrawHitSphere>();
    Math::Vector4 color =
        p.value("color", Math::Vector4{0.2f, 0.85f, 1.0f, 1.0f});
    cmp->SetColor(color);
    cmp->SetLightingEnabled(p.value("isLightingEnabled", false));
    return;
  }

  if (slot.typeName == "CmpRotator") {
    if (!entity.HasUpdCmp<CmpRotator>()) {
      return;
    }
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
    if (!entity.HasUpdCmp<CmpScroll>()) {
      return;
    }
    CmpScroll *cmp = entity.GetUpdCmp<CmpScroll>();
    cmp->SetScrollSpeed(p.value("scrollSpeed", 0.0f));
    return;
  }

  if (slot.typeName == "CmpHitSphere") {
    if (!entity.HasUpdCmp<CmpHitSphere>()) {
      return;
    }
    CmpHitSphere *cmp = entity.GetUpdCmp<CmpHitSphere>();
    std::vector<HitSphere> spheres;
    if (p.contains("spheres")) {
      ReadHitSpheresFromJson(p.at("spheres"), spheres);
    }
    cmp->SetSpheres(spheres);
    cmp->SetIsActive(p.value("isActive", true));
    return;
  }

  if (slot.typeName == "CmpEditorAssetPath") {
    if (!entity.HasDataCmp<CmpEditorAssetPath>()) {
      return;
    }
    CmpEditorAssetPath *cmp = entity.GetDataCmp<CmpEditorAssetPath>();
    cmp->SetAssetPath(p.value("assetPath", std::string{}));
    cmp->SetLoadObjAsMesh(p.value("loadObjAsMesh", true));
    cmp->SetLoadImageAsBillboard(p.value("loadImageAsBillboard", true));
    cmp->SetBillboardSize(p.value("billboardWidth", 1.0f),
                          p.value("billboardHeight", 1.0f));
  }
}

namespace {

std::string ToLowerAscii(std::string value) {
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

bool IsImageAssetExtension(const std::string &extensionLower) {
  return extensionLower == ".png" || extensionLower == ".jpg" ||
         extensionLower == ".jpeg";
}

} // namespace

void TryLoadEditorAssetPathPreview(Entity &entity) {
  if (!entity.HasDataCmp<CmpEditorAssetPath>()) {
    return;
  }
  const CmpEditorAssetPath *data = entity.GetDataCmp<CmpEditorAssetPath>();
  if (!data->LoadObjAsMesh()) {
    return;
  }
  const std::string path = data->AssetPath();
  if (path.empty()) {
    return;
  }

  namespace fs = std::filesystem;
  const fs::path assetPath(path);
  if (!fs::exists(assetPath)) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine == nullptr) {
    return;
  }

  const std::string extLower = ToLowerAscii(assetPath.extension().string());

  if (data->LoadObjAsMesh() && extLower == ".obj") {
    const std::string objDirectory = assetPath.parent_path().generic_string();
    const std::string objFileName = assetPath.filename().string();
    const std::string mtlFileName = assetPath.stem().string() + ".mtl";

    const std::shared_ptr<AssetModel> model =
        engine->GetModel(objDirectory, objFileName, objDirectory, mtlFileName);
    if (model) {
      if (!entity.HasDrawCmp<CmpMesh>()) {
        entity.AddDrawCmp<CmpMesh>();
      }
      CmpMesh *mesh = entity.GetDrawCmp<CmpMesh>();
      if (mesh != nullptr) {
        mesh->SetModel(model.get());
        mesh->SetLightingEnabled(true);
      }
    }
  }

  if (data->LoadImageAsBillboard() && IsImageAssetExtension(extLower)) {
    const uint32_t textureIndex = engine->GetTexture(path);
    if (!entity.HasDrawCmp<CmpEditorTextureBillboard>()) {
      entity.AddDrawCmp<CmpEditorTextureBillboard>();
    }
    CmpEditorTextureBillboard *billboard =
        entity.GetDrawCmp<CmpEditorTextureBillboard>();
    if (billboard != nullptr) {
      billboard->SetTextureIndex(textureIndex);
      billboard->SetSize(data->BillboardWidth(), data->BillboardHeight());
    }
  }
}

std::unique_ptr<Entity> BuildEntityFromDefinition(
    const EntityDefinition &definition) {
  auto entity = std::make_unique<Entity>(definition.entityName);
  entity->SetInitialTransform(definition.initialTransform);

  std::vector<ComponentSlot> orderedSlots = definition.components;
  std::stable_sort(orderedSlots.begin(), orderedSlots.end(),
                   [](const ComponentSlot &a, const ComponentSlot &b) {
                     return a.executionOrder < b.executionOrder;
                   });

  for (const ComponentSlot &slot : orderedSlots) {
    if (!EntityHasComponentForSlot(*entity, slot)) {
      if (!TryAddEmptyComponent(*entity, slot)) {
        continue;
      }
    }
    ApplyComponentParams(*entity, slot);
    ApplySlotExecutionOrder(*entity, slot);
  }

  entity->SortComponents();

  TryLoadEditorAssetPathPreview(*entity);

  if (!definition.tagHashes.empty()) {
    entity->SetTagHashes(definition.tagHashes);
  } else {
    entity->SetTagsFromNames(definition.tags);
  }
  return entity;
}

} // namespace

bool TryLoadSceneDocument(const std::string &jsonFilePath,
                          EditorSceneDocument &outDocument) {
  namespace fs = std::filesystem;
  if (!fs::exists(jsonFilePath)) {
    return false;
  }

  std::ifstream file(jsonFilePath);
  if (!file.is_open()) {
    return false;
  }

  nlohmann::json json{};
  file >> json;

  if (json.contains("entities") && json.at("entities").is_array()) {
    outDocument = json.get<EditorSceneDocument>();
    return true;
  }

  outDocument = EditorSceneDocument{};
  outDocument.sceneName = "EditorScene";
  SceneEntityInstance instance{};
  instance.definition = json.get<EntityDefinition>();
  instance.instanceName = instance.definition.entityName;
  if (instance.instanceName.empty()) {
    instance.instanceName = "Entity_0";
  }
  outDocument.instances = {std::move(instance)};
  return true;
}

std::vector<std::unique_ptr<Entity>>
BuildEntitiesFromSceneDocument(const EditorSceneDocument &document) {
  std::vector<std::unique_ptr<Entity>> entities;
  entities.reserve(document.instances.size());

  for (const SceneEntityInstance &instance : document.instances) {
    entities.push_back(BuildEntityFromDefinition(instance.definition));
  }

  for (size_t i = 0; i < document.instances.size(); ++i) {
    const int parentIndex = document.instances[i].parentIndex;
    if (parentIndex < 0 ||
        parentIndex >= static_cast<int>(entities.size()) ||
        static_cast<size_t>(parentIndex) == i) {
      continue;
    }
    entities[i]->SetParent(entities[static_cast<size_t>(parentIndex)].get());
  }

  for (size_t i = 0; i < document.instances.size(); ++i) {
    if (document.instances[i].parentIndex >= 0) {
      continue;
    }
    entities[i]->ResolveDependencies();
    entities[i]->Initialize();
  }

  return entities;
}

} // namespace Atrum::EntityEditor
