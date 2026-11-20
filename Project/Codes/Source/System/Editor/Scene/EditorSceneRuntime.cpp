#include "System/Editor/Scene/EditorSceneRuntime.h"

#include "Camera/StageViewFramingBounds.h"
#include "Engine/AtrumEngine.h"
#include "System/Entity/Components/CmpDrawHitSphere.h"
#include "System/Entity/Components/CmpColliderAabb.h"
#include "System/Entity/Components/CmpEditorAssetPath.h"
#include "System/Entity/Components/CmpRuntimeTexturePath.h"
#include "System/Entity/Components/CmpHitSphere.h" // HitSphere
#include "System/Entity/Components/CmpEditorTextureBillboard.h"
#include "System/Entity/Components/CmpMesh.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpBillboardInstancedGrid.h"
#include "System/Entity/Components/CmpBallAxisAlignedBounce.h"
#include "System/Entity/Components/CmpBallBounceImpactFx.h"
#include "System/Entity/Components/CmpBallKinematics.h"
#include "System/Entity/Components/CmpBounceBall.h"
#include "System/Entity/Components/CmpCamera.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
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
    if (slot.typeName == "CmpBillboardInstancedGrid") {
      return entity.HasDrawCmp<CmpBillboardInstancedGrid>();
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
    if (slot.typeName == "CmpCamera") {
      return entity.HasUpdCmp<CmpCamera>();
    }
    if (slot.typeName == "CmpParticleEmitter") {
      return entity.HasUpdCmp<CmpParticleEmitter>();
    }
    if (slot.typeName == "CmpBounceBall") {
      return entity.HasUpdCmp<CmpBounceBall>();
    }
    if (slot.typeName == "CmpBallKinematics") {
      return entity.HasUpdCmp<CmpBallKinematics>();
    }
    if (slot.typeName == "CmpBallAxisAlignedBounce") {
      return entity.HasUpdCmp<CmpBallAxisAlignedBounce>();
    }
    if (slot.typeName == "CmpBallBounceImpactFx") {
      return entity.HasUpdCmp<CmpBallBounceImpactFx>();
    }
  }
  if (slot.kind == ComponentSlotKind::Data) {
    if (slot.typeName == "CmpEditorAssetPath") {
      return entity.HasDataCmp<CmpEditorAssetPath>();
    }
    if (slot.typeName == "CmpRuntimeTexturePath") {
      return entity.HasDataCmp<CmpRuntimeTexturePath>();
    }
    if (slot.typeName == "CmpColliderAabb") {
      return entity.HasDataCmp<CmpColliderAabb>();
    }
  }
  return false;
}

bool TryAddEmptyComponent(Entity &entity, const ComponentSlot &slot) {
  if (slot.typeName == "CmpMeshSphere" && slot.kind == ComponentSlotKind::Draw) {
    entity.AddDrawCmp<CmpMeshSphere>();
    return true;
  }
  if (slot.typeName == "CmpBillboardInstancedGrid" &&
      slot.kind == ComponentSlotKind::Draw) {
    entity.AddDrawCmp<CmpBillboardInstancedGrid>();
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
  if (slot.typeName == "CmpCamera" && slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpCamera>();
    return true;
  }
  if (slot.typeName == "CmpParticleEmitter" &&
      slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpParticleEmitter>();
    return true;
  }
  if (slot.typeName == "CmpBounceBall" && slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpBounceBall>();
    return true;
  }
  if (slot.typeName == "CmpBallKinematics" && slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpBallKinematics>();
    return true;
  }
  if (slot.typeName == "CmpBallAxisAlignedBounce" &&
      slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpBallAxisAlignedBounce>();
    return true;
  }
  if (slot.typeName == "CmpBallBounceImpactFx" &&
      slot.kind == ComponentSlotKind::Upd) {
    entity.AddUpdCmp<CmpBallBounceImpactFx>();
    return true;
  }
  if (slot.typeName == "CmpEditorAssetPath" &&
      slot.kind == ComponentSlotKind::Data) {
    entity.AddDataCmp<CmpEditorAssetPath>();
    return true;
  }
  if (slot.typeName == "CmpRuntimeTexturePath" &&
      slot.kind == ComponentSlotKind::Data) {
    entity.AddDataCmp<CmpRuntimeTexturePath>();
    return true;
  }
  if (slot.typeName == "CmpColliderAabb" &&
      slot.kind == ComponentSlotKind::Data) {
    entity.AddDataCmp<CmpColliderAabb>();
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

  if (slot.typeName == "CmpBillboardInstancedGrid") {
    if (!entity.HasDrawCmp<CmpBillboardInstancedGrid>()) {
      return;
    }
    CmpBillboardInstancedGrid *cmp =
        entity.GetDrawCmp<CmpBillboardInstancedGrid>();
    cmp->SetTextureFilePath(p.value("textureFilePath", std::string{}));
    cmp->SetGridCountX(p.value("gridCountX", 10));
    cmp->SetGridCountY(p.value("gridCountY", 10));
    cmp->SetSpacing(p.value("spacing", 1.35f));
    cmp->SetCellFill(p.value("cellFill", 0.78f));
    cmp->SetUseAutoSizeInCell(p.value("useAutoSizeInCell", true));
    cmp->SetCheckerboardEnabled(p.value("checkerboardEnabled", true));
    cmp->SetCheckerColorA(
        p.value("checkerColorA", Math::Vector4{0.92f, 0.96f, 1.05f, 1.0f}));
    cmp->SetCheckerColorB(
        p.value("checkerColorB", Math::Vector4{0.42f, 0.52f, 0.68f, 1.0f}));
    cmp->SetDrawCellGridLines(p.value("drawCellGridLines", true));
    cmp->SetGridLineColor(
        p.value("gridLineColor", Math::Vector4{0.35f, 0.4f, 0.48f, 0.95f}));
    cmp->SetGridPlaneYOffset(p.value("gridPlaneYOffset", -0.55f));
    cmp->SetBillboardWidth(p.value("billboardWidth", 0.85f));
    cmp->SetBillboardHeight(p.value("billboardHeight", 0.85f));
    cmp->SetBaseColor(p.value("baseColor", Math::Vector4::White()));
    return;
  }

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
    return;
  }

  if (slot.typeName == "CmpRuntimeTexturePath") {
    if (!entity.HasDataCmp<CmpRuntimeTexturePath>()) {
      return;
    }
    CmpRuntimeTexturePath *cmp = entity.GetDataCmp<CmpRuntimeTexturePath>();
    cmp->SetTexturePath(p.value("texturePath", std::string{}));
    return;
  }

  if (slot.typeName == "CmpColliderAabb") {
    if (!entity.HasDataCmp<CmpColliderAabb>()) {
      return;
    }
    CmpColliderAabb *cmp = entity.GetDataCmp<CmpColliderAabb>();
    cmp->SetLocalCenter(p.value("localCenter", Math::Vector3::Zero()));
    cmp->SetHalfExtents(
        p.value("halfExtents", Math::Vector3{0.5f, 0.5f, 0.5f}));
    cmp->SetTrigger(p.value("isTrigger", false));
    return;
  }

  if (slot.typeName == "CmpCamera") {
    if (!entity.HasUpdCmp<CmpCamera>()) {
      return;
    }
    CmpCamera *cmp = entity.GetUpdCmp<CmpCamera>();
    cmp->SetPerspectiveParams(p.value("perspectiveFovY", 0.5f),
                              p.value("nearClip", 0.125f),
                              p.value("farClip", 128.0f));
    return;
  }

  if (slot.typeName == "CmpParticleEmitter") {
    if (!entity.HasUpdCmp<CmpParticleEmitter>()) {
      return;
    }
    CmpParticleEmitter *cmp = entity.GetUpdCmp<CmpParticleEmitter>();
    cmp->SetEmitterName(p.value("emitterName", std::string{}));
    cmp->SetJsonFilePath(p.value("jsonFilePath", std::string{}));
    std::vector<std::string> samplePaths;
    if (p.contains("samplePaths") && p.at("samplePaths").is_array()) {
      for (const nlohmann::json &item : p.at("samplePaths")) {
        if (item.is_string()) {
          samplePaths.push_back(item.get<std::string>());
        }
      }
    }
    cmp->SetSamplePaths(std::move(samplePaths));
    cmp->SetAutoCarousel(p.value("autoCarousel", false));
    cmp->SetCarouselIntervalSeconds(p.value("carouselIntervalSeconds", 5.0f));
    cmp->SetFollowOwnerTransform(p.value("followOwnerTransform", true));
    return;
  }

  if (slot.typeName == "CmpBounceBall") {
    if (!entity.HasUpdCmp<CmpBounceBall>()) {
      return;
    }
    CmpBounceBall *cmp = entity.GetUpdCmp<CmpBounceBall>();
    cmp->SetInitialVelocity(p.value("velocity", Math::Vector3{7.0f, 5.5f, 0.0f}));
    cmp->SetGravity(p.value("gravity", -14.0f));
    cmp->SetLeftWallX(p.value("leftWallX", -5.0f));
    cmp->SetRightWallX(p.value("rightWallX", 5.0f));
    cmp->SetGroundY(p.value("groundY", -3.68f));
    cmp->SetRadius(p.value("radius", 0.32f));
    cmp->SetWallRestitution(p.value("wallRestitution", 0.94f));
    cmp->SetFloorRestitution(p.value("floorRestitution", 0.88f));
    cmp->SetImpactJsonPath(
        p.value("impactJsonPath", std::string{"demos/ball_wall_shockwave.json"}));
    cmp->SetImpactCooldownSeconds(p.value("impactCooldownSeconds", 0.12f));
    cmp->EnsureAndSyncChildComponents();
    return;
  }

  if (slot.typeName == "CmpBallKinematics") {
    if (!entity.HasUpdCmp<CmpBallKinematics>()) {
      return;
    }
    CmpBallKinematics *cmp = entity.GetUpdCmp<CmpBallKinematics>();
    cmp->SetInitialVelocity(p.value("velocity", Math::Vector3{7.0f, 5.5f, 0.0f}));
    cmp->SetGravity(p.value("gravity", -14.0f));
    return;
  }

  if (slot.typeName == "CmpBallAxisAlignedBounce") {
    if (!entity.HasUpdCmp<CmpBallAxisAlignedBounce>()) {
      return;
    }
    CmpBallAxisAlignedBounce *cmp =
        entity.GetUpdCmp<CmpBallAxisAlignedBounce>();
    cmp->SetLeftWallX(p.value("leftWallX", -5.0f));
    cmp->SetRightWallX(p.value("rightWallX", 5.0f));
    cmp->SetGroundY(p.value("groundY", -3.68f));
    cmp->SetRadius(p.value("radius", 0.32f));
    cmp->SetWallRestitution(p.value("wallRestitution", 0.94f));
    cmp->SetFloorRestitution(p.value("floorRestitution", 0.88f));
    return;
  }

  if (slot.typeName == "CmpBallBounceImpactFx") {
    if (!entity.HasUpdCmp<CmpBallBounceImpactFx>()) {
      return;
    }
    CmpBallBounceImpactFx *cmp = entity.GetUpdCmp<CmpBallBounceImpactFx>();
    cmp->SetImpactJsonPath(
        p.value("impactJsonPath", std::string{"demos/ball_wall_shockwave.json"}));
    cmp->SetImpactCooldownSeconds(p.value("impactCooldownSeconds", 0.12f));
    return;
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

  InitializeSceneDocumentEntities(entities);

  return entities;
}

void InitializeSceneDocumentEntities(
    std::vector<std::unique_ptr<Entity>> &entities) {
  for (std::unique_ptr<Entity> &entity : entities) {
    if (entity) {
      entity->ResolveDependencies();
    }
  }
  for (std::unique_ptr<Entity> &entity : entities) {
    if (entity && entity->GetParent() == nullptr) {
      entity->Initialize();
    }
  }
  for (std::unique_ptr<Entity> &entity : entities) {
    if (entity && entity->GetParent() != nullptr &&
        entity->GetState() != Entity::State::Active) {
      entity->Initialize();
    }
  }
}

void DrawSceneDocumentEntities(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  for (const std::unique_ptr<Entity> &entity : entities) {
    if (!entity || entity->GetParent() != nullptr) {
      continue;
    }
    if (entity->GetState() == Entity::State::Active) {
      entity->Draw();
    }
  }
  for (const std::unique_ptr<Entity> &entity : entities) {
    if (!entity || entity->GetParent() == nullptr) {
      continue;
    }
    if (entity->GetState() != Entity::State::Active) {
      continue;
    }
    Entity *parent = entity->GetParent();
    if (parent == nullptr || parent->GetState() != Entity::State::Active) {
      entity->Draw();
    }
  }
}

StageViewFramingBounds
ResolveStageViewFramingBounds(const EditorSceneDocument &document) {
  if (document.stageViewFramingBounds.has_value()) {
    return *document.stageViewFramingBounds;
  }

  StageViewFramingBounds bounds =
      StageViewFramingBounds::DefaultFromStageConstants();

  bool hasLeft = false;
  bool hasRight = false;
  bool hasGround = false;
  bool hasTop = false;
  float leftX = bounds.leftEndX;
  float rightX = bounds.rightEndX;
  float groundY = bounds.groundHeight;
  float topY = bounds.barrageSpawnTopY;

  for (const SceneEntityInstance &instance : document.instances) {
    const auto applyMarker = [&](const std::string &name) -> bool {
      return instance.instanceName == name ||
             instance.definition.entityName == name;
    };

    const float px = instance.definition.initialTransform.translate.x;
    const float py = instance.definition.initialTransform.translate.y;

    if (applyMarker("LeftBoundMarker")) {
      leftX = px;
      hasLeft = true;
    }
    if (applyMarker("RightBoundMarker")) {
      rightX = px;
      hasRight = true;
    }
    if (applyMarker("GroundBoundMarker")) {
      groundY = py;
      hasGround = true;
    }
    if (applyMarker("TopBoundMarker")) {
      topY = py;
      hasTop = true;
    }
  }

  if (hasLeft && hasRight) {
    bounds.leftEndX = std::min(leftX, rightX);
    bounds.rightEndX = std::max(leftX, rightX);
  }
  if (hasGround) {
    bounds.groundHeight = groundY;
  }
  if (hasTop) {
    bounds.barrageSpawnTopY = topY;
  }
  if (hasTop && hasGround && bounds.barrageSpawnTopY <= bounds.groundHeight) {
    return StageViewFramingBounds::DefaultFromStageConstants();
  }
  if (hasLeft && hasRight && bounds.rightEndX <= bounds.leftEndX) {
    return StageViewFramingBounds::DefaultFromStageConstants();
  }

  return bounds;
}

} // namespace Atrum::EntityEditor
