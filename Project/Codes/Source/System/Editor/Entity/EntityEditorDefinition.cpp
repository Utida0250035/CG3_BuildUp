#include "System/Editor/Entity/EntityEditorDefinition.h"

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <algorithm>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

namespace {

NLOHMANN_JSON_SERIALIZE_ENUM(ComponentSlotKind,
                            {{ComponentSlotKind::Upd, "Upd"},
                             {ComponentSlotKind::Draw, "Draw"},
                             {ComponentSlotKind::Data, "Data"}})

} // namespace

bool IsKnownEditorRuntimeComponentTypeName(const std::string &typeName) {
  return typeName == "CmpMeshSphere" || typeName == "CmpMesh" ||
         typeName == "CmpEditorTextureBillboard" || typeName == "CmpDrawHitSphere" ||
         typeName == "CmpRotator" || typeName == "CmpScroll" ||
         typeName == "CmpHitSphere" || typeName == "CmpEditorAssetPath" ||
         typeName == "CmpRuntimeTexturePath" || typeName == "CmpColliderAabb" ||
         typeName == "CmpCamera" || typeName == "CmpParticleEmitter" ||
         typeName == "CmpBounceBall" || typeName == "CmpBallKinematics" ||
         typeName == "CmpBallAxisAlignedBounce" ||
         typeName == "CmpBallBounceImpactFx" ||
         typeName == "CmpBillboardInstancedGrid";
}

ComponentSlotKind DefaultKindForTypeName(const std::string &typeName) {
  if (typeName == "CmpMeshSphere" || typeName == "CmpMesh" ||
      typeName == "CmpEditorTextureBillboard" || typeName == "CmpDrawHitSphere" ||
      typeName == "CmpBillboardInstancedGrid") {
    return ComponentSlotKind::Draw;
  }
  if (typeName == "CmpRotator" || typeName == "CmpScroll" ||
      typeName == "CmpHitSphere" || typeName == "CmpCamera" ||
      typeName == "CmpParticleEmitter" || typeName == "CmpBounceBall" ||
      typeName == "CmpBallKinematics" ||
      typeName == "CmpBallAxisAlignedBounce" ||
      typeName == "CmpBallBounceImpactFx") {
    return ComponentSlotKind::Upd;
  }
  if (typeName == "CmpEditorAssetPath" || typeName == "CmpRuntimeTexturePath" ||
      typeName == "CmpColliderAabb") {
    return ComponentSlotKind::Data;
  }
  return ComponentSlotKind::Draw;
}

nlohmann::json DefaultParamsForType(const std::string &typeName) {
  if (typeName == "CmpMeshSphere") {
    return {{"radius", 1.0f},
            {"subdivision", 12},
            {"isLightingEnabled", true},
            {"color", Math::Vector4::White()}};
  }
  if (typeName == "CmpMesh") {
    return {{"radius", 0.5f},
            {"isLightingEnabled", true},
            {"color", Math::Vector4::White()},
            {"modelAssetPath", ""}};
  }
  if (typeName == "CmpEditorTextureBillboard") {
    return {{"textureFilePath", "./Assets/Images/white4x4.png"},
            {"billboardWidth", 1.0f},
            {"billboardHeight", 1.0f}};
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
  if (typeName == "CmpEditorAssetPath") {
    return {{"assetPath", ""},
            {"loadObjAsMesh", true},
            {"loadImageAsBillboard", true},
            {"billboardWidth", 1.0f},
            {"billboardHeight", 1.0f}};
  }
  if (typeName == "CmpRuntimeTexturePath") {
    return {{"texturePath", "./Assets/Images/white4x4.png"}};
  }
  if (typeName == "CmpColliderAabb") {
    return {{"localCenter", Math::Vector3::Zero()},
            {"halfExtents", Math::Vector3{0.5f, 0.5f, 0.5f}},
            {"isTrigger", false}};
  }
  if (typeName == "CmpCamera") {
    return {{"perspectiveFovY", 0.5f},
            {"nearClip", 0.125f},
            {"farClip", 128.0f}};
  }
  if (typeName == "CmpParticleEmitter") {
    return {{"emitterName", ""},
            {"jsonFilePath", "samples/01_burst_directional_fountain.json"},
            {"samplePaths", nlohmann::json::array()},
            {"autoCarousel", false},
            {"carouselIntervalSeconds", 5.0f},
            {"followOwnerTransform", true}};
  }
  if (typeName == "CmpBillboardInstancedGrid") {
    return {{"textureFilePath", "./Assets/Images/white4x4.png"},
            {"gridCountX", 10},
            {"gridCountY", 10},
            {"spacing", 1.35f},
            {"cellFill", 0.78f},
            {"useAutoSizeInCell", true},
            {"checkerboardEnabled", true},
            {"checkerColorA", Math::Vector4{0.92f, 0.96f, 1.05f, 1.0f}},
            {"checkerColorB", Math::Vector4{0.42f, 0.52f, 0.68f, 1.0f}},
            {"drawCellGridLines", true},
            {"gridLineColor", Math::Vector4{0.35f, 0.4f, 0.48f, 0.95f}},
            {"gridPlaneYOffset", -0.55f},
            {"billboardWidth", 0.85f},
            {"billboardHeight", 0.85f},
            {"baseColor", Math::Vector4::White()}};
  }
  if (typeName == "CmpBounceBall") {
    return {{"velocity", Math::Vector3{7.0f, 5.5f, 0.0f}},
            {"gravity", -14.0f},
            {"leftWallX", -5.0f},
            {"rightWallX", 5.0f},
            {"groundY", -3.68f},
            {"radius", 0.32f},
            {"wallRestitution", 0.94f},
            {"floorRestitution", 0.88f},
            {"impactJsonPath", "demos/ball_wall_shockwave.json"},
            {"impactCooldownSeconds", 0.12f}};
  }
  if (typeName == "CmpBallKinematics") {
    return {{"velocity", Math::Vector3{7.0f, 5.5f, 0.0f}}, {"gravity", -14.0f}};
  }
  if (typeName == "CmpBallAxisAlignedBounce") {
    return {{"leftWallX", -5.0f},
            {"rightWallX", 5.0f},
            {"groundY", -3.68f},
            {"radius", 0.32f},
            {"wallRestitution", 0.94f},
            {"floorRestitution", 0.88f}};
  }
  if (typeName == "CmpBallBounceImpactFx") {
    return {{"impactJsonPath", "demos/ball_wall_shockwave.json"},
            {"impactCooldownSeconds", 0.12f}};
  }
  return nlohmann::json::object();
}

EntityDefinition MakeDefaultEntityDefinition(const std::string &entityName) {
  EntityDefinition definition{};
  definition.entityName = entityName;
  ComponentSlot meshSlot{};
  meshSlot.kind = ComponentSlotKind::Draw;
  meshSlot.typeName = "CmpMeshSphere";
  meshSlot.params = DefaultParamsForType("CmpMeshSphere");
  ComponentSlot rotatorSlot{};
  rotatorSlot.kind = ComponentSlotKind::Upd;
  rotatorSlot.typeName = "CmpRotator";
  rotatorSlot.params = DefaultParamsForType("CmpRotator");
  definition.components = {meshSlot, rotatorSlot};
  return definition;
}

void to_json(nlohmann::json &j, const ComponentSlot &slot) {
  j = nlohmann::json{{"kind", slot.kind},
                     {"typeName", slot.typeName},
                     {"executionOrder", slot.executionOrder},
                     {"params", slot.params}};
}

void from_json(const nlohmann::json &j, ComponentSlot &slot) {
  slot.typeName = j.value("typeName", slot.typeName);
  slot.executionOrder = j.value("executionOrder", slot.executionOrder);
  if (j.contains("kind") && !j.at("kind").is_null()) {
    from_json(j.at("kind"), slot.kind);
  }
  if (IsKnownEditorRuntimeComponentTypeName(slot.typeName)) {
    slot.kind = DefaultKindForTypeName(slot.typeName);
  } else if (!j.contains("kind") || j.at("kind").is_null()) {
    slot.kind = ComponentSlotKind::Draw;
  }
  slot.params = j.value("params", nlohmann::json::object());
}

namespace {

void MigrateLegacyEntityTypeField(const nlohmann::json &j,
                                  EntityDefinition &definition) {
  if (!j.contains("entityType")) {
    return;
  }
  const std::string legacyType = j.at("entityType").get<std::string>();
  if (legacyType.empty() || legacyType == "NONE") {
    return;
  }
  if (std::find(definition.tags.begin(), definition.tags.end(), legacyType) ==
      definition.tags.end()) {
    definition.tags.push_back(legacyType);
  }
}

} // namespace

void to_json(nlohmann::json &j, const EntityDefinition &definition) {
  j = nlohmann::json{{"entityName", definition.entityName},
                     {"initialTransform", definition.initialTransform},
                     {"tags", definition.tags},
                     {"tagHashes", definition.tagHashes},
                     {"components", definition.components}};
}

void from_json(const nlohmann::json &j, EntityDefinition &definition) {
  definition.entityName = j.value("entityName", definition.entityName);
  definition.initialTransform =
      j.value("initialTransform", definition.initialTransform);
  definition.tags = j.value("tags", definition.tags);
  definition.tagHashes = j.value("tagHashes", definition.tagHashes);
  definition.components = j.value("components", definition.components);
  MigrateLegacyEntityTypeField(j, definition);
}

void to_json(nlohmann::json &j, const SceneEntityInstance &instance) {
  j = nlohmann::json{{"instanceName", instance.instanceName},
                     {"parentIndex", instance.parentIndex},
                     {"definition", instance.definition}};
}

void from_json(const nlohmann::json &j, SceneEntityInstance &instance) {
  instance.instanceName = j.value("instanceName", instance.instanceName);
  instance.parentIndex = j.value("parentIndex", instance.parentIndex);
  if (j.contains("definition")) {
    instance.definition = j.at("definition").get<EntityDefinition>();
  } else {
    instance.definition = j.get<EntityDefinition>();
    if (instance.instanceName.empty()) {
      instance.instanceName = instance.definition.entityName;
    }
  }
}

void to_json(nlohmann::json &j, const EditorSceneDocument &document) {
  j = nlohmann::json{{"sceneName", document.sceneName},
                     {"tagCatalog", document.tagCatalog},
                     {"entities", document.instances}};
  if (document.stageViewFramingBounds.has_value()) {
    const StageViewFramingBounds &bounds = *document.stageViewFramingBounds;
    j["stageViewFramingBounds"] =
        nlohmann::json{{"leftEndX", bounds.leftEndX},
                       {"rightEndX", bounds.rightEndX},
                       {"groundHeight", bounds.groundHeight},
                       {"barrageSpawnTopY", bounds.barrageSpawnTopY},
                       {"pivotZ", bounds.pivotZ}};
  }
}

void from_json(const nlohmann::json &j, EditorSceneDocument &document) {
  document.sceneName = j.value("sceneName", document.sceneName);
  document.tagCatalog = j.value("tagCatalog", document.tagCatalog);
  document.instances = j.value("entities", document.instances);
  document.stageViewFramingBounds.reset();
  if (j.contains("stageViewFramingBounds")) {
    StageViewFramingBounds parsed{};
    if (StageViewFramingBounds::TryParse(j.at("stageViewFramingBounds"),
                                         parsed)) {
      document.stageViewFramingBounds = parsed;
    }
  }
}

} // namespace Atrum::EntityEditor
