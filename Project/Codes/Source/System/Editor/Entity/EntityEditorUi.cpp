#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
#include "System/Editor/Entity/EntityEditorAuthoringEditorAssetPathEditNotify.h"
#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsBallSplit.h"
#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsCmpCamera.h"
#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsPlayable.h"
#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsGenericJson.h"
#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsExternalOpen.h"
#include "System/Editor/Entity/EntityEditorComponentSettingsCopyPaste.h"
#include "System/Editor/Entity/EntityEditorComponentSlotParamsNormalize.h"

#include "System/Editor/EditorEntityWorkbench.h"
#include "System/Editor/EditorFrame.h"
#include "System/Editor/EditorSceneRunStatus.h"
#include "Scene/CommandChangeScene.h"
#include "System/Editor/Prefab/PrefabAsset.h"
#include "System/Editor/Prefab/PrefabAssetBatchSpawnUi.h"
#include "System/Editor/Prefab/PrefabAssetProjectPanelList.h"
#include "System/Editor/Prefab/PrefabAssetOverwriteSaveUi.h"
#include "System/Editor/Prefab/PrefabAssetCollisionPolicy.h"
#include "System/Editor/Prefab/PrefabAssetOutputCollisionPolicyUi.h"
#include "System/Editor/Prefab/PrefabAssetAutoStemRenameReport.h"
#include "System/Editor/Entity/EntityEditorPrefabBatchSpawnFromFile.h"
#include "System/Editor/Entity/EntityEditorTagCatalog.h"
#include "System/Editor/EditorImportedAsset.h"
#include "System/Editor/EditorImportedAssetBatchApplyToSelection.h"
#include "System/Editor/EditorImportedAssetDropPolicyUi.h"
#include "System/Editor/EditorImportedAssetProjectPanelList.h"
#include "System/Editor/Entity/EntityEditorReleaseStartupBuildUp.h"
#include "Scene/SceneEntityEditorScenePlayBuildUpRoute.h"
#include "Scene/SceneEntityEditorInEditorPlayQueue.h"
#include "System/Editor/EditorDebugPlayAreaScaffold.h"
#include "System/Editor/Entity/EntityEditorSceneSwitchUnsavedPrompt.h"
#include "System/Editor/Entity/EntityEditorUiEditableSceneList.h"
#include "System/Editor/Entity/EditorScenePlayRequestBuildUpQueue.h"
#include "System/Editor/Entity/EntityEditorStartupSceneSetWithoutAutosave.h"
#include "System/Editor/EditorLaunchConfigLoadDiagnostics.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/GameAssetsPaths.h"
#include "System/Hash64.h"
#include "ForDebug/ImGui.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Editor/Entity/EntityEditorHierarchyDragDrop.h"
#include "System/Editor/Entity/EntityEditorInspectorSceneJsonPathUi.h"
#include "System/Editor/Entity/EntityEditorInspectorInstanceIdentityUi.h"
#include "System/Editor/Entity/EntityEditorInspectorTransformCopyPasteUi.h"
#include "System/Editor/Entity/EntityEditorTransformCopyPaste.h"
#include "System/Editor/Entity/EntityEditorClearAllEntityTags.h"
#include "System/Editor/Entity/EntityEditorInspectorComponentRemoveBatchUi.h"
#include "System/Editor/Entity/EntityEditorInspectorExecutionOrderBatchUi.h"
#include "System/Editor/Entity/EntityEditorHierarchyAddObjectUi.h"
#include "System/Editor/Entity/EntityEditorInspectorComponentTypeComboApply.h"
#include "System/Editor/Entity/EntityEditorInspectorImportedDropBatchApplyUi.h"

#include <algorithm>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Atrum::EntityEditor {

namespace {

struct KnownComponentEntry {
  const char *typeName;
  ComponentSlotKind kind;
};

constexpr KnownComponentEntry kKnownComponents[] = {
    {"CmpMeshSphere", ComponentSlotKind::Draw},
    {"CmpMesh", ComponentSlotKind::Draw},
    {"CmpEditorTextureBillboard", ComponentSlotKind::Draw},
    {"CmpDrawHitSphere", ComponentSlotKind::Draw},
    {"CmpEditorAssetPath", ComponentSlotKind::Data},
    {"CmpRuntimeTexturePath", ComponentSlotKind::Data},
    {"CmpColliderAabb", ComponentSlotKind::Data},
    {"CmpRotator", ComponentSlotKind::Upd},
    {"CmpScroll", ComponentSlotKind::Upd},
    {"CmpHitSphere", ComponentSlotKind::Upd},
    {"CmpCamera", ComponentSlotKind::Upd},
    {"CmpParticleEmitter", ComponentSlotKind::Upd},
    {"CmpBounceBall", ComponentSlotKind::Upd},
    {"CmpBallKinematics", ComponentSlotKind::Upd},
    {"CmpBallAxisAlignedBounce", ComponentSlotKind::Upd},
    {"CmpBallBounceImpactFx", ComponentSlotKind::Upd},
    {"CmpBillboardInstancedGrid", ComponentSlotKind::Draw},
    {"CmpLuaUpd", ComponentSlotKind::Upd},
    {"CmpLuaDraw", ComponentSlotKind::Draw},
    {"AbstractInput", ComponentSlotKind::Upd},
    {"CmpMoveByInput2D", ComponentSlotKind::Upd},
    {"CmpDrawPlayerParryField", ComponentSlotKind::Draw},
    {"CmpDirection", ComponentSlotKind::Data},
    {"CmpLhRigidBody", ComponentSlotKind::Upd},
    {"CmpHealthHit", ComponentSlotKind::Upd},
    {"CmpSwing", ComponentSlotKind::Upd},
    {"CmpChildEntitys", ComponentSlotKind::Upd},
    {"CmpParry", ComponentSlotKind::Data},
    {"CmpStgPlayerPilot", ComponentSlotKind::Upd},
};

void AppendKnownComponentSlot(EntityDefinition &definition,
                              const KnownComponentEntry &entry) {
  ComponentSlot slot{};
  slot.kind = entry.kind;
  slot.typeName = entry.typeName;
  slot.params = DefaultParamsForType(entry.typeName);
  NormalizeKnownComponentSlotParams(slot);
  definition.components.push_back(std::move(slot));
}

bool DrawVector3Param(const char *label, nlohmann::json &params,
                      const char *key, const Math::Vector3 &fallback) {
  Math::Vector3 v = params.value(key, fallback);
  const bool changed = ImGui::DragFloat3(label, &v.x, 0.02f);
  if (changed) {
    params[key] = v;
  }
  return changed;
}

bool DrawVector4ColorParam(const char *label, nlohmann::json &params,
                             const char *key, const Math::Vector4 &fallback) {
  Math::Vector4 color = params.value(key, fallback);
  const bool changed = ImGui::ColorEdit4(label, &color.x);
  if (changed) {
    params[key] = color;
  }
  return changed;
}

constexpr const char *kDeleteScenePopupId = "シーン削除の確認";

void QueueDeleteSceneConfirm(UiState &ui, const std::string &path) {
  if (path.empty()) {
    return;
  }
  ui.pendingDeleteScenePath = path;
  ui.requestDeleteSceneConfirm = true;
}

void DrawDeleteSceneConfirmPopup(EntityEditor &editor) {
  UiState &ui = editor.Ui();
  if (ui.requestDeleteSceneConfirm) {
    ImGui::OpenPopup(kDeleteScenePopupId);
    ui.requestDeleteSceneConfirm = false;
  }

  if (ImGui::BeginPopupModal(kDeleteScenePopupId, nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextUnformatted("次のシーン JSON を削除しますか？");
    ImGui::Spacing();
    ImGui::TextWrapped("%s", ui.pendingDeleteScenePath.c_str());
    ImGui::Spacing();
    if (ImGui::Button("削除", ImVec2(120.0f, 0.0f))) {
      editor.DeleteSceneFile(ui.pendingDeleteScenePath);
      ui.pendingDeleteScenePath.clear();
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("キャンセル", ImVec2(120.0f, 0.0f))) {
      ui.pendingDeleteScenePath.clear();
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

bool DrawComponentParams(ComponentSlot &slot, UiState &ui) {
  nlohmann::json &p = slot.params;
  bool changed = false;

  bool ballSplitChanged = false;
  if (TryDrawBallSplitComponentParams(slot, ballSplitChanged)) {
    return ballSplitChanged;
  }

  bool cmpCameraChanged = false;
  if (TryDrawCmpCameraComponentParams(slot, cmpCameraChanged)) {
    return cmpCameraChanged;
  }

  if (slot.typeName == "CmpRuntimeTexturePath") {
    std::string texturePath = p.value("texturePath", std::string{});
    changed |= ImGui::InputString("texturePath", texturePath);
    if (changed) {
      p["texturePath"] = texturePath;
    }
    if (!ui.lastImportedAssetPath.empty() &&
        ImGui::Button("直近ドロップを texturePath に適用")) {
      p["texturePath"] = ui.lastImportedAssetPath;
      changed = true;
    }
    EntityEditorUiDrawComponentParamsExternalOpen::DrawButton(texturePath, ui);
    return changed;
  }

  if (slot.typeName == "CmpColliderAabb") {
    Math::Vector3 center = p.value("localCenter", Math::Vector3::Zero());
    if (ImGui::DragFloat3("localCenter", &center.x, 0.02f)) {
      p["localCenter"] = center;
      changed = true;
    }
    Math::Vector3 halfExtents =
        p.value("halfExtents", Math::Vector3{0.5f, 0.5f, 0.5f});
    if (ImGui::DragFloat3("halfExtents", &halfExtents.x, 0.02f, 0.01f)) {
      p["halfExtents"] = halfExtents;
      changed = true;
    }
    bool isTrigger = p.value("isTrigger", false);
    if (ImGui::Checkbox("isTrigger", &isTrigger)) {
      p["isTrigger"] = isTrigger;
      changed = true;
    }
    return changed;
  }

  if (slot.typeName == "CmpEditorAssetPath") {
    ImGui::TextDisabled(
        "プレビュー表示には CmpMesh または CmpEditorTextureBillboard の "
        "Draw スロットを別途追加してください");
    std::string path = p.value("assetPath", std::string{});
    changed |= ImGui::InputString("assetPath", path);
    if (changed) {
      p["assetPath"] = path;
    }
    bool loadMesh = p.value("loadObjAsMesh", true);
    if (ImGui::Checkbox("loadObjAsMesh", &loadMesh)) {
      p["loadObjAsMesh"] = loadMesh;
      changed = true;
    }
    bool loadImage = p.value("loadImageAsBillboard", true);
    if (ImGui::Checkbox("loadImageAsBillboard", &loadImage)) {
      p["loadImageAsBillboard"] = loadImage;
      changed = true;
    }
    float width = p.value("billboardWidth", 1.0f);
    if (ImGui::DragFloat("billboardWidth", &width, 0.02f, 0.01f, 32.0f)) {
      p["billboardWidth"] = width;
      changed = true;
    }
    float height = p.value("billboardHeight", 1.0f);
    if (ImGui::DragFloat("billboardHeight", &height, 0.02f, 0.01f, 32.0f)) {
      p["billboardHeight"] = height;
      changed = true;
    }
    if (!ui.lastImportedAssetPath.empty() &&
        ImGui::Button("直近ドロップを assetPath に適用")) {
      p["assetPath"] = ui.lastImportedAssetPath;
      changed = true;
    }
    EntityEditorUiDrawComponentParamsExternalOpen::DrawButton(path, ui);
    return changed;
  }

  if (slot.typeName == "CmpMesh") {
    float radius = p.value("radius", 0.5f);
    if (ImGui::DragFloat("radius", &radius, 0.02f, 0.01f, 64.0f)) {
      p["radius"] = radius;
      changed = true;
    }
    bool lighting = p.value("isLightingEnabled", true);
    if (ImGui::Checkbox("isLightingEnabled", &lighting)) {
      p["isLightingEnabled"] = lighting;
      changed = true;
    }
    changed |= DrawVector4ColorParam("color", p, "color", Math::Vector4::White());
    std::string modelPath = p.value("modelAssetPath", std::string{});
    if (ImGui::InputString("modelAssetPath", modelPath)) {
      p["modelAssetPath"] = modelPath;
      changed = true;
    }
    if (!ui.lastImportedAssetPath.empty() &&
        ImGui::Button("直近ドロップを modelAssetPath に適用")) {
      p["modelAssetPath"] = ui.lastImportedAssetPath;
      changed = true;
    }
    EntityEditorUiDrawComponentParamsExternalOpen::DrawButton(modelPath, ui);
    return changed;
  }

  if (slot.typeName == "CmpEditorTextureBillboard") {
    std::string tex = p.value("textureFilePath", std::string{});
    changed |= ImGui::InputString("textureFilePath", tex);
    if (changed) {
      p["textureFilePath"] = tex;
    }
    float width = p.value("billboardWidth", 1.0f);
    if (ImGui::DragFloat("billboardWidth", &width, 0.02f, 0.01f, 32.0f)) {
      p["billboardWidth"] = width;
      changed = true;
    }
    float height = p.value("billboardHeight", 1.0f);
    if (ImGui::DragFloat("billboardHeight", &height, 0.02f, 0.01f, 32.0f)) {
      p["billboardHeight"] = height;
      changed = true;
    }
    if (!ui.lastImportedAssetPath.empty() &&
        ImGui::Button("直近ドロップを textureFilePath に適用")) {
      p["textureFilePath"] = ui.lastImportedAssetPath;
      changed = true;
    }
    EntityEditorUiDrawComponentParamsExternalOpen::DrawButton(tex, ui);
    return changed;
  }

  if (slot.typeName == "CmpMeshSphere") {
    float radius = p.value("radius", 1.0f);
    if (ImGui::DragFloat("radius", &radius, 0.02f, 0.01f, 64.0f)) {
      p["radius"] = radius;
      changed = true;
    }
    int subdivision = static_cast<int>(p.value("subdivision", 8u));
    if (ImGui::DragInt("subdivision", &subdivision, 1, 3, 32)) {
      p["subdivision"] = static_cast<uint32_t>(subdivision);
      changed = true;
    }
    bool lighting = p.value("isLightingEnabled", true);
    if (ImGui::Checkbox("isLightingEnabled", &lighting)) {
      p["isLightingEnabled"] = lighting;
      changed = true;
    }
    changed |= DrawVector4ColorParam("color", p, "color", Math::Vector4::White());
    return changed;
  }

  if (slot.typeName == "CmpDrawHitSphere") {
    ImGui::TextDisabled(
        "描画には同一エンティティの CmpHitSphere (Upd) が必要です");
    changed |= DrawVector4ColorParam("color", p, "color",
                                     Math::Vector4{0.2f, 0.85f, 1.0f, 1.0f});
    bool lighting = p.value("isLightingEnabled", false);
    if (ImGui::Checkbox("isLightingEnabled", &lighting)) {
      p["isLightingEnabled"] = lighting;
      changed = true;
    }
    return changed;
  }

  if (slot.typeName == "CmpRotator") {
    changed |= DrawVector3Param("rotateAxis", p, "rotateAxis", Math::Vector3::UpLh());
    float speed = p.value("rotateSpeed", 0.0f);
    if (ImGui::DragFloat("rotateSpeed", &speed, 0.05f, -64.0f, 64.0f)) {
      p["rotateSpeed"] = speed;
      changed = true;
    }
    bool pivotIsOwner = p.value("pivotIsOwner", true);
    if (ImGui::Checkbox("pivotIsOwner", &pivotIsOwner)) {
      p["pivotIsOwner"] = pivotIsOwner;
      changed = true;
    }
    if (pivotIsOwner) {
      ImGui::TextDisabled("プレビューではオーナー Entity をピボットにします");
    } else {
      ImGui::TextDisabled("ピボット未設定の場合、CmpRotator は動作しません");
    }
    return changed;
  }

  if (slot.typeName == "CmpScroll") {
    float speed = p.value("scrollSpeed", 0.0f);
    if (ImGui::DragFloat("scrollSpeed", &speed, 0.05f, -64.0f, 64.0f)) {
      p["scrollSpeed"] = speed;
      changed = true;
    }
    return changed;
  }

  if (slot.typeName == "CmpBillboardInstancedGrid") {
    std::string tex = p.value("textureFilePath", std::string{});
    changed |= ImGui::InputString("textureFilePath", tex);
    if (changed) {
      p["textureFilePath"] = tex;
    }
    EntityEditorUiDrawComponentParamsExternalOpen::DrawButton(tex, ui);
    int gridX = p.value("gridCountX", 10);
    if (ImGui::DragInt("gridCountX", &gridX, 1, 1, 32)) {
      p["gridCountX"] = gridX;
      changed = true;
    }
    int gridY = p.value("gridCountY", 10);
    if (ImGui::DragInt("gridCountY", &gridY, 1, 1, 32)) {
      p["gridCountY"] = gridY;
      changed = true;
    }
    float spacing = p.value("spacing", 1.15f);
    if (ImGui::DragFloat("spacing", &spacing, 0.01f, 0.1f, 8.0f)) {
      p["spacing"] = spacing;
      changed = true;
    }
    float w = p.value("billboardWidth", 0.85f);
    if (ImGui::DragFloat("billboardWidth", &w, 0.01f, 0.05f, 8.0f)) {
      p["billboardWidth"] = w;
      changed = true;
    }
    float h = p.value("billboardHeight", 0.85f);
    if (ImGui::DragFloat("billboardHeight", &h, 0.01f, 0.05f, 8.0f)) {
      p["billboardHeight"] = h;
      changed = true;
    }
    changed |= DrawVector4ColorParam("baseColor", p, "baseColor",
                                     Math::Vector4::White());
    return changed;
  }

  if (slot.typeName == "CmpParticleEmitter") {
    std::string path = p.value("jsonFilePath", std::string{});
  changed |= ImGui::InputString("jsonFilePath", path);
    if (changed) {
      p["jsonFilePath"] = path;
    }
    std::string emitterName = p.value("emitterName", std::string{});
    changed |= ImGui::InputString("emitterName", emitterName);
    if (changed) {
      p["emitterName"] = emitterName;
    }
    bool autoCarousel = p.value("autoCarousel", false);
    if (ImGui::Checkbox("autoCarousel", &autoCarousel)) {
      p["autoCarousel"] = autoCarousel;
      changed = true;
    }
    if (autoCarousel) {
      ImGui::TextDisabled(
          "sample を画面内の別位置へ、インデックスをずらして同時再生");
    }
    float interval = p.value("carouselIntervalSeconds", 5.0f);
    if (ImGui::DragFloat("carouselIntervalSeconds", &interval, 0.1f, 0.5f,
                         120.0f)) {
      p["carouselIntervalSeconds"] = interval;
      changed = true;
    }
    bool follow = p.value("followOwnerTransform", true);
    if (ImGui::Checkbox("followOwnerTransform", &follow)) {
      p["followOwnerTransform"] = follow;
      changed = true;
    }
    const int sampleCount =
        p.contains("samplePaths") && p.at("samplePaths").is_array()
            ? static_cast<int>(p.at("samplePaths").size())
            : 0;
    ImGui::Text("samplePaths: %d 件（JSON で編集）", sampleCount);
    return changed;
  }

  if (slot.typeName == "CmpBounceBall") {
    changed |= DrawVector3Param("velocity", p, "velocity", {7.0f, 5.5f, 0.0f});
    float gravity = p.value("gravity", -14.0f);
    if (ImGui::DragFloat("gravity", &gravity, 0.1f, -80.0f, 0.0f)) {
      p["gravity"] = gravity;
      changed = true;
    }
    float leftWall = p.value("leftWallX", -5.0f);
    if (ImGui::DragFloat("leftWallX", &leftWall, 0.05f, -32.0f, 32.0f)) {
      p["leftWallX"] = leftWall;
      changed = true;
    }
    float rightWall = p.value("rightWallX", 5.0f);
    if (ImGui::DragFloat("rightWallX", &rightWall, 0.05f, -32.0f, 32.0f)) {
      p["rightWallX"] = rightWall;
      changed = true;
    }
    float groundY = p.value("groundY", -3.68f);
    if (ImGui::DragFloat("groundY", &groundY, 0.05f, -16.0f, 16.0f)) {
      p["groundY"] = groundY;
      changed = true;
    }
    float radius = p.value("radius", 0.32f);
    if (ImGui::DragFloat("radius", &radius, 0.01f, 0.05f, 4.0f)) {
      p["radius"] = radius;
      changed = true;
    }
    std::string impactPath = p.value("impactJsonPath", std::string{});
    changed |= ImGui::InputString("impactJsonPath", impactPath);
    if (changed) {
      p["impactJsonPath"] = impactPath;
    }
    return changed;
  }

  if (slot.typeName == "CmpHitSphere") {
    bool active = p.value("isActive", true);
    if (ImGui::Checkbox("isActive", &active)) {
      p["isActive"] = active;
      changed = true;
    }
    if (!p.contains("spheres") || !p.at("spheres").is_array()) {
      p["spheres"] = nlohmann::json::array();
    }
    nlohmann::json &spheres = p.at("spheres");
    if (spheres.empty()) {
      spheres.push_back(
          {{"localOffset", Math::Vector3::Zero()}, {"radius", 0.5f}});
    }

    static int selectedSphereIndex = 0;
    const int sphereCount = static_cast<int>(spheres.size());
    selectedSphereIndex =
        std::clamp(selectedSphereIndex, 0, std::max(0, sphereCount - 1));

    if (ImGui::Button("球を追加")) {
      spheres.push_back(
          {{"localOffset", Math::Vector3::Zero()}, {"radius", 0.5f}});
      selectedSphereIndex = sphereCount;
      changed = true;
    }
    ImGui::SameLine();
    if (sphereCount > 1 &&
        ImGui::Button("選択した球を削除")) {
      spheres.erase(spheres.begin() + selectedSphereIndex);
      selectedSphereIndex =
          std::clamp(selectedSphereIndex, 0,
                     std::max(0, static_cast<int>(spheres.size()) - 1));
      changed = true;
    }

    for (int i = 0; i < sphereCount; ++i) {
      const std::string label = "spheres[" + std::to_string(i) + "]";
      if (ImGui::Selectable(label.c_str(), selectedSphereIndex == i)) {
        selectedSphereIndex = i;
      }
    }

    if (!spheres.empty()) {
      nlohmann::json &sphere = spheres.at(static_cast<size_t>(selectedSphereIndex));
      const std::string indexSuffix =
          "[" + std::to_string(selectedSphereIndex) + "]";

      Math::Vector3 offset =
          sphere.value("localOffset", Math::Vector3::Zero());
      const std::string offsetLabel = "localOffset" + indexSuffix;
      if (ImGui::DragFloat3(offsetLabel.c_str(), &offset.x, 0.02f)) {
        sphere["localOffset"] = offset;
        changed = true;
      }
      float radius = sphere.value("radius", 0.5f);
      const std::string radiusLabel = "radius" + indexSuffix;
      if (ImGui::DragFloat(radiusLabel.c_str(), &radius, 0.02f, 0.01f,
                           64.0f)) {
        sphere["radius"] = radius;
        changed = true;
      }
    }
    return changed;
  }

  {
    bool playableChanged = false;
    if (EntityEditorUiDrawComponentParamsPlayable::TryDraw(slot,
                                                           playableChanged)) {
      return playableChanged;
    }
  }

  if (slot.typeName == "CmpLuaUpd") {
    bool changed = false;
    std::string scriptPath = slot.params.value("scriptPath", std::string{});
    if (ImGui::InputText("scriptPath", &scriptPath)) {
      slot.params["scriptPath"] = scriptPath;
      changed = true;
    }
    std::string updGroup = slot.params.value("updCmpGroup", "MOVEMENT");
    if (ImGui::InputText("updCmpGroup", &updGroup)) {
      slot.params["updCmpGroup"] = updGroup;
      changed = true;
    }
    return changed;
  }

  if (slot.typeName == "CmpLuaDraw") {
    bool changed = false;
    std::string scriptPath = slot.params.value("scriptPath", std::string{});
    if (ImGui::InputText("scriptPath", &scriptPath)) {
      slot.params["scriptPath"] = scriptPath;
      changed = true;
    }
    std::string drawGroup = slot.params.value("drawCmpGroup", "NONE");
    if (ImGui::InputText("drawCmpGroup", &drawGroup)) {
      slot.params["drawCmpGroup"] = drawGroup;
      changed = true;
    }
    return changed;
  }

  bool genericChanged = false;
  if (EntityEditorUiDrawComponentParamsGenericJson::TryDrawUnknownComponentParams(
          slot, genericChanged)) {
    return genericChanged;
  }

  ImGui::TextDisabled("未対応コンポーネント: %s", slot.typeName.c_str());
  return false;
}

int ComponentSlotKindToKindComboIndex(const ComponentSlotKind kind) {
  switch (kind) {
  case ComponentSlotKind::Draw:
    return 1;
  case ComponentSlotKind::Data:
    return 2;
  case ComponentSlotKind::Upd:
  default:
    return 0;
  }
}

ComponentSlotKind ComponentSlotKindFromKindComboIndex(const int kindIndex) {
  switch (kindIndex) {
  case 1:
    return ComponentSlotKind::Draw;
  case 2:
    return ComponentSlotKind::Data;
  case 0:
  default:
    return ComponentSlotKind::Upd;
  }
}

std::vector<int> CollectComponentIndicesForKind(
    const EntityDefinition &document, const ComponentSlotKind kind) {
  std::vector<int> indices;
  for (int i = 0; i < static_cast<int>(document.components.size()); ++i) {
    if (document.components[static_cast<size_t>(i)].kind == kind) {
      indices.push_back(i);
    }
  }
  return indices;
}

void CollectKnownPresetIndicesForKind(const ComponentSlotKind kind,
                                      std::vector<int> &outPresetIndices) {
  outPresetIndices.clear();
  const int presetCount = static_cast<int>(std::size(kKnownComponents));
  for (int i = 0; i < presetCount; ++i) {
    if (kKnownComponents[i].kind == kind) {
      outPresetIndices.push_back(i);
    }
  }
}

bool IsInstanceIndexSelected(const UiState &ui, const int index) {
  if (ui.selectedInstanceIndices.empty()) {
    return ui.selectedInstanceIndex == index;
  }
  return std::find(ui.selectedInstanceIndices.begin(),
                   ui.selectedInstanceIndices.end(),
                   index) != ui.selectedInstanceIndices.end();
}

void SetSingleInstanceSelection(UiState &ui, const int index) {
  ui.selectedInstanceIndex = index;
  ui.selectedInstanceIndices = {index};
}

void ToggleInstanceSelection(UiState &ui, const int index) {
  if (ui.selectedInstanceIndices.empty()) {
    ui.selectedInstanceIndices = {ui.selectedInstanceIndex};
  }
  const auto found = std::find(ui.selectedInstanceIndices.begin(),
                               ui.selectedInstanceIndices.end(), index);
  if (found != ui.selectedInstanceIndices.end()) {
    if (ui.selectedInstanceIndices.size() <= 1) {
      return;
    }
    ui.selectedInstanceIndices.erase(found);
    ui.selectedInstanceIndex = ui.selectedInstanceIndices.front();
    return;
  }
  ui.selectedInstanceIndices.push_back(index);
  std::sort(ui.selectedInstanceIndices.begin(),
            ui.selectedInstanceIndices.end());
  ui.selectedInstanceIndex = index;
}

void DrawMultiInstanceBatchPanel(EntityEditor &editor,
                                 const std::vector<int> &selection,
                                 bool &changed) {
  EditorSceneDocument &scene = editor.SceneDocument();
  UiState &ui = editor.Ui();

  ImGui::Text("複数選択: %zu 件", selection.size());
  ImGui::TextDisabled("Ctrl+クリックで選択を追加/解除");

  static float batchScaleFactor = 1.0f;
  ImGui::InputFloat("スケール倍率", &batchScaleFactor, 0.01f, 0.1f, "%.3f");
  if (ImGui::Button("スケール一括適用")) {
    for (const int index : selection) {
      if (index < 0 || index >= static_cast<int>(scene.instances.size())) {
        continue;
      }
      Math::Vector3 &scale =
          scene.instances[static_cast<size_t>(index)]
              .definition.initialTransform.scale;
      scale.x *= batchScaleFactor;
      scale.y *= batchScaleFactor;
      scale.z *= batchScaleFactor;
    }
    changed = true;
    batchScaleFactor = 1.0f;
  }

  ImGui::Separator();
  if (ImGui::Button("先頭選択のトランスフォームをコピー")) {
    if (!selection.empty() && selection.front() >= 0 &&
        selection.front() < static_cast<int>(scene.instances.size())) {
      const Math::TransformLH &source =
          scene.instances[static_cast<size_t>(selection.front())]
              .definition.initialTransform;
      EntityEditorTransformCopyPaste::CopyInitialTransformToClipboard(ui,
                                                                      source);
      ui.importStatusMessage = "クリップボードへ初期トランスフォームをコピー";
    }
  }
  ImGui::BeginDisabled(!ui.clipboardInitialTransform.has_value());
  if (ImGui::Button("クリップボードから一括適用")) {
    const int applied =
        EntityEditorTransformCopyPaste::ApplyClipboardTransformToInstanceIndices(
            editor, selection);
    if (applied > 0) {
      changed = true;
      ui.importStatusMessage =
          "初期トランスフォームを " + std::to_string(applied) + " 体へ適用";
    }
  }
  ImGui::EndDisabled();

  static float positionOffset[3] = {0.0f, 0.0f, 0.0f};
  ImGui::InputFloat3("位置オフセット", positionOffset);
  if (ImGui::Button("位置オフセット一括適用")) {
    for (const int index : selection) {
      if (index < 0 || index >= static_cast<int>(scene.instances.size())) {
        continue;
      }
      Math::Vector3 &translate =
          scene.instances[static_cast<size_t>(index)]
              .definition.initialTransform.translate;
      translate.x += positionOffset[0];
      translate.y += positionOffset[1];
      translate.z += positionOffset[2];
    }
    changed = true;
    positionOffset[0] = 0.0f;
    positionOffset[1] = 0.0f;
    positionOffset[2] = 0.0f;
  }

  ImGui::Separator();
  ImGui::InputString("一括付与タグ", ui.newEntityTagName);
  ImGui::SameLine();
    if (ImGui::Button("タグをすべて解除（選択全体）")) {
      const int cleared =
          EntityEditorClearAllEntityTags::ClearAllTagsOnInstanceIndices(scene,
                                                                      selection);
      if (cleared > 0) {
        changed = true;
        ui.importStatusMessage =
            "タグをすべて解除: " + std::to_string(cleared) + " 体";
      }
    }

    if (ImGui::Button("タグを一括付与")) {
      if (!ui.newEntityTagName.empty()) {
        for (const int index : selection) {
        if (index < 0 || index >= static_cast<int>(scene.instances.size())) {
          continue;
        }
        AddTagToCatalog(scene, ui.newEntityTagName);
        AddTagToEntityDefinition(
            scene.instances[static_cast<size_t>(index)].definition,
            ui.newEntityTagName);
      }
      ui.newEntityTagName.clear();
      changed = true;
    }
  }
}

void DrawHierarchyNode(EntityEditor &editor, const int nodeIndex) {
  EditorSceneDocument &scene = editor.SceneDocument();
  UiState &ui = editor.Ui();
  if (nodeIndex < 0 ||
      nodeIndex >= static_cast<int>(scene.instances.size())) {
    return;
  }
  SceneEntityInstance &instance =
      scene.instances[static_cast<size_t>(nodeIndex)];
  const bool selected = IsInstanceIndexSelected(ui, nodeIndex);
  ImGui::PushID(nodeIndex);
  if (ImGui::Selectable(instance.instanceName.c_str(), selected)) {
    if (ImGui::GetIO().KeyCtrl) {
      ToggleInstanceSelection(ui, nodeIndex);
    } else {
      SetSingleInstanceSelection(ui, nodeIndex);
    }
  }
  EntityEditorHierarchyDragDrop::AttachToHierarchyNode(editor, nodeIndex);
  ImGui::PopID();

  for (int i = 0; i < static_cast<int>(scene.instances.size()); ++i) {
    if (scene.instances[static_cast<size_t>(i)].parentIndex == nodeIndex) {
      ImGui::Indent();
      DrawHierarchyNode(editor, i);
      ImGui::Unindent();
    }
  }
}

void SyncSelectedComponentToKind(UiState &ui, const EntityDefinition &document) {
  const std::vector<int> kindIndices =
      CollectComponentIndicesForKind(document, ui.selectedComponentKind);
  if (kindIndices.empty()) {
    return;
  }
  if (std::find(kindIndices.begin(), kindIndices.end(),
                ui.selectedComponentIndex) == kindIndices.end()) {
    ui.selectedComponentIndex = kindIndices.front();
  }
}

} // namespace

void EntityEditor::LayoutEditorWorkbench() {
  Atrum::Editor::LayoutEntityEditorWorkbench(*this);
}

void EntityEditor::DrawHierarchyPanel() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  const Atrum::Editor::EditorSceneRunPhase runPhase =
      Atrum::Editor::QueryEntityEditorSceneRunPhase(
      ui_, Atrum::Editor::EditorFrame::Instance().SceneViewTabSelection(),
      Atrum::CommandChangeScene::Instance()->GetIsExecute(),
      ui_.requestPlayRuntimeScene);
  Atrum::Editor::DrawEntityEditorSceneModeToolbar(*this, runPhase);
#else
  ImGui::TextDisabled("モード: %s",
                      ui_.interactionMode == EditorInteractionMode::Play
                          ? "実行"
                          : "シーン");
#endif

  ImGui::Separator();
  ImGui::InputString("シーン名", scene_.sceneName);

  EntityEditorHierarchyAddObjectUi::Draw(*this);
  ImGui::SameLine();
  if (!scene_.instances.empty() && ImGui::Button("複製")) {
    DuplicateSelectedInstance();
  }
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
    ImGui::SetTooltip("Ctrl+D");
  }
  ImGui::SameLine();
  if (scene_.instances.size() > 1 && ImGui::Button("削除")) {
    RemoveSelectedInstance();
  }

  ImGui::Separator();
  ImGui::TextDisabled(
      "ドラッグ&ドロップ: 親変更 | Shift+ドロップ: 並べ替え | Ctrl+ドロップ: 複製");
  if (ImGui::TreeNodeEx(scene_.sceneName.c_str(),
                        ImGuiTreeNodeFlags_DefaultOpen)) {
    EntityEditorHierarchyDragDrop::AttachRootDropTarget(*this);
    for (int i = 0; i < static_cast<int>(scene_.instances.size()); ++i) {
      if (scene_.instances[static_cast<size_t>(i)].parentIndex < 0) {
        DrawHierarchyNode(*this, i);
      }
    }
    ImGui::TreePop();
  }

  if (ImGui::IsAnyItemDeactivatedAfterEdit()) {
    EntityEditorAuthoringDocumentEditNotify::Notify(*this);
  }
}

void EntityEditor::DrawProjectPanel() {
  Atrum::Editor::Scene::EnsureScenesDirectoryExists();

  ImGui::TextUnformatted("シーン一覧");
  Editor::EditorDebugPlayAreaScaffold::DrawProjectPanelControls(*this);
  if (ImGui::Button("新規シーン")) {
    CreateNewSceneFile();
  }
  ImGui::SameLine();
  if (ImGui::Button("シーンを実行")) {
    QueueEditorScenePlayViaBuildUp(ui_);
  }
  ImGui::SameLine();
  if (ImGui::Button("エディタ内プレビュー")) {
    QueueInEditorPlayRuntimeTabOnly(ui_);
  }
  ImGui::SameLine();
  if (ImGui::Button("SceneBuildUp")) {
    ui_.requestSaveToDisk = true;
    ui_.sceneBuildUpLaunchPath.clear();
    ui_.requestLaunchSceneBuildUp = true;
  }
  if (ImGui::Button("起動シーンに設定")) {
    EntityEditorStartupSceneSetWithoutAutosave::SetOnly(*this,
                                                       &ui_.importStatusMessage);
  }
  ImGui::SameLine();
  if (ImGui::Button("設定して BuildUp")) {
    if (!TrySetStartupSceneAndQueueBuildUp(*this, &ui_.importStatusMessage)) {
      // importStatusMessage は TrySet 内で設定済み
    }
  }
  const Atrum::Editor::LaunchConfigLoadDiagnostics::LaunchConfigResolveResult
      startup =
          Atrum::Editor::LaunchConfigLoadDiagnostics::
              ResolveStartupScenePathDetailed();
  if (!startup.startupScenePath.empty()) {
    ImGui::TextDisabled("Release 起動: %s", startup.startupScenePath.c_str());
    if (ImGui::Button("Release 起動で BuildUp")) {
      if (!TryQueueBuildUpFromReleaseStartup(ui_, &ui_.importStatusMessage)) {
        // importStatusMessage は TryQueue 内で設定済み
      }
    }
  } else {
    ImGui::TextDisabled(
        "Release 起動: %s（%s）",
        Atrum::Editor::LaunchConfigLoadDiagnostics::
            DescribeLaunchConfigLoadIssueJa(startup.issue),
        startup.configPathTried.empty() ? "—"
                                        : startup.configPathTried.c_str());
  }
  ImGui::Separator();

  ImGui::TextUnformatted("ファイルドロップ（obj / png 等）");
  ImGui::TextDisabled("ウィンドウへドロップ → %s",
                      Atrum::Editor::Import::kImportedAssetsDirectory);
  Atrum::Editor::EditorImportedAssetDropPolicyUi::DrawDropCollisionPolicyControls(
      ui_);
  if (!ui_.recentImportedAssetPaths.empty()) {
    ImGui::Text("ドロップキュー: %zu 件（最新は末尾）",
                ui_.recentImportedAssetPaths.size());
    for (const std::string &importedPath : ui_.recentImportedAssetPaths) {
      ImGui::BulletText("%s", importedPath.c_str());
    }
  } else if (!ui_.lastImportedAssetPath.empty()) {
    ImGui::TextWrapped("直近: %s", ui_.lastImportedAssetPath.c_str());
  }
  if (!ui_.importStatusMessage.empty()) {
    ImGui::TextWrapped("%s", ui_.importStatusMessage.c_str());
  }
  Atrum::Editor::EditorImportedAssetBatchApplyToSelection::
      DrawProjectPanelBatchApplyUi(*this);
  Atrum::Editor::EditorImportedAssetProjectPanelList::DrawImportedAssetsFolderList(
      *this);
  ImGui::Separator();

  DrawEntityEditorEditableSceneList(*this);

  ImGui::Separator();
  if (!ui_.selectedScenePath.empty()) {
    if (ImGui::Button("シーンを開く")) {
      TryOpenSceneFromPath(ui_.selectedScenePath);
    }
    ImGui::SameLine();
    if (ImGui::Button("シーンを削除")) {
      QueueDeleteSceneConfirm(ui_, ui_.selectedScenePath);
    }
  }

  DrawDeleteSceneConfirmPopup(*this);
  DrawEntityEditorSceneSwitchUnsavedPrompt(*this);

  ImGui::Spacing();
  ImGui::TextUnformatted("プレハブ一覧 (Entity JSON)");
  ImGui::Separator();
  Atrum::Editor::Prefab::PrefabAssetOutputCollisionPolicyUi::
      DrawPrefabOutputCollisionPolicyControls(ui_);

  if (!scene_.instances.empty() &&
      ImGui::Button("選択エンティティをプレハブ JSON へ出力")) {
    const std::string entityName = SelectedDefinition().entityName;
    const std::string path =
        Atrum::Editor::Prefab::ResolvePrefabJsonOutputPath(
            entityName, ui_.prefabOutputCollisionPolicy,
            ui_.prefabOutputBatchNamePrefix);
    if (!path.empty() && SaveSelectedInstanceToPrefabPath(path)) {
      ui_.selectedPrefabPath = path;
      ui_.importStatusMessage = "プレハブ出力: " + path;
      if (Atrum::Editor::Prefab::PrefabAssetAutoStemRenameReport::
              UsedAutoStemIncrementRename(entityName, path)) {
        ui_.importStatusMessage += "（自動リネーム stem_N）";
      }
    } else if (!path.empty()) {
      ui_.importStatusMessage = "プレハブ出力失敗: " + path;
    }
  }
  Atrum::Editor::Prefab::PrefabAssetOverwriteSaveUi::
      DrawOverwriteSelectedPrefabSaveButton(*this);
  ImGui::Spacing();

  const std::vector<std::string> prefabPaths =
      Atrum::Editor::Prefab::ListPrefabJsonFiles();
  if (prefabPaths.empty()) {
    ImGui::TextDisabled("プレハブが見つかりません: %s",
                        Atrum::Editor::Prefab::kEntitiesPrefabDirectory);
    return;
  }

  Atrum::Editor::Prefab::PrefabAssetProjectPanelList::DrawPrefabJsonFileList(
      *this, prefabPaths);

  ImGui::Separator();
  Atrum::Editor::Prefab::PrefabAssetBatchSpawnUi::DrawBatchSpawnControls(*this);
  if (!ui_.selectedPrefabPath.empty()) {
    const int count = std::max(1, ui_.prefabSpawnBatchCount);
    const char *addLabel =
        count > 1 ? "シーンへ追加（上記件数）" : "シーンへ追加（1 体）";
    if (ImGui::Button(addLabel)) {
      const int spawned =
          EntityEditor::EntityEditorPrefabBatchSpawnFromFile::
              AppendFromPrefabFileBatch(*this, ui_.selectedPrefabPath, count,
                                        ui_.prefabSpawnSpacingX);
      if (spawned > 0) {
        ui_.importStatusMessage =
            "プレハブ追加: " + std::to_string(spawned) + " 体";
      }
    }
  }
}

void EntityEditor::DrawInspectorPanel() {
  bool scenePathChanged = false;
  EntityEditorInspectorSceneJsonPathUi::Draw(*this, scenePathChanged);
  if (scenePathChanged) {
    EntityEditorAuthoringDocumentEditNotify::Notify(*this);
  }
  ImGui::Separator();

  if (scene_.instances.empty()) {
    ImGui::TextUnformatted("シーンにオブジェクトがありません");
    return;
  }

  ClampSelectedInstanceIndex();
  if (ui_.selectedInstanceIndices.size() > 1) {
    bool batchChanged = false;
    DrawMultiInstanceBatchPanel(*this, ui_.selectedInstanceIndices, batchChanged);
    if (batchChanged) {
      EntityEditorAuthoringDocumentEditNotify::Notify(*this);
    }
    return;
  }

  bool changed = false;
  SceneEntityInstance &instance = SelectedInstance();
  EntityDefinition &document = SelectedDefinition();

  bool identityChanged = false;
  EntityEditorInspectorInstanceIdentityUi::Draw(*this, instance, document,
                                                identityChanged);
  changed |= identityChanged;

  int parentIndex = instance.parentIndex;
  const int selfIndex = ui_.selectedInstanceIndex;
  if (ImGui::BeginCombo("親オブジェクト",
                         parentIndex < 0 ? "なし"
                                         : scene_.instances[static_cast<size_t>(
                                               parentIndex)]
                                               .instanceName.c_str())) {
    if (ImGui::Selectable("なし", parentIndex < 0)) {
      instance.parentIndex = -1;
      changed = true;
    }
    for (int i = 0; i < static_cast<int>(scene_.instances.size()); ++i) {
      if (i == selfIndex) {
        continue;
      }
      const bool picked = parentIndex == i;
      if (ImGui::Selectable(scene_.instances[static_cast<size_t>(i)]
                                .instanceName.c_str(),
                            picked)) {
        instance.parentIndex = i;
        changed = true;
      }
    }
    ImGui::EndCombo();
  }

  ImGui::Separator();

  if (ImGui::CollapsingHeader("タグ", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::TextDisabled("JSON: tags + tagHashes / 実行時 std::set<uint64_t>");
    ImGui::TextUnformatted("役割タグ（Assets/Tags/tagRegistry.json）");
    EntityTagRegistry::Instance().EnsureLoaded();
    for (const std::string &roleTagName :
         EntityTagRegistry::Instance().RoleTagNames()) {
      const bool hasRole = std::find(document.tags.begin(), document.tags.end(),
                                     roleTagName) != document.tags.end();
      bool roleEnabled = hasRole;
      if (ImGui::Checkbox(roleTagName.c_str(), &roleEnabled)) {
        if (roleEnabled) {
          AddTagToCatalog(scene_, roleTagName);
          AddTagToEntityDefinition(document, roleTagName);
        } else {
          RemoveTagFromEntityDefinition(document, roleTagName);
        }
        changed = true;
      }
    }
    ImGui::Separator();

    for (size_t tagIndex = 0; tagIndex < document.tags.size(); ++tagIndex) {
      ImGui::PushID(static_cast<int>(tagIndex));
      const std::string tagNameBefore = document.tags[tagIndex];
      std::string &tagName = document.tags[tagIndex];
      if (ImGui::InputString("名前", tagName)) {
        if (tagName != tagNameBefore) {
          EditEntityDefinitionTagAt(document, tagIndex, tagName);
        }
        changed = true;
      }
      ImGui::Text("hash: 0x%016llX",
                  static_cast<unsigned long long>(
                      TagHashForDefinitionName(tagName)));
      if (ImGui::Button("削除")) {
        RemoveTagFromEntityDefinition(document, tagName);
        changed = true;
        ImGui::PopID();
        break;
      }
      ImGui::Separator();
      ImGui::PopID();
    }

    ImGui::InputString("新規タグ名", ui_.newEntityTagName);
    ImGui::SameLine();
    if (ImGui::Button("タグを追加")) {
      if (!ui_.newEntityTagName.empty()) {
        AddTagToCatalog(scene_, ui_.newEntityTagName);
        AddTagToEntityDefinition(document, ui_.newEntityTagName);
        ui_.newEntityTagName.clear();
        changed = true;
      }
    }

    const std::vector<std::string> assignable = scene_.tagCatalog;
    static int addExistingTagIndex = 0;
    if (!assignable.empty()) {
      addExistingTagIndex =
          std::clamp(addExistingTagIndex, 0,
                     static_cast<int>(assignable.size()) - 1);
      std::vector<const char *> labels;
      labels.reserve(assignable.size());
      for (const std::string &name : assignable) {
        labels.push_back(name.c_str());
      }
      ImGui::Combo("カタログから追加", &addExistingTagIndex, labels.data(),
                   static_cast<int>(labels.size()));
      if (ImGui::Button("カタログのタグを付与")) {
        const std::string &picked = assignable[static_cast<size_t>(
            addExistingTagIndex)];
        AddTagToEntityDefinition(document, picked);
        changed = true;
      }
    }

    if (ImGui::Button("タグをすべて解除")) {
      EntityEditorClearAllEntityTags::ClearAllTagsOnDefinition(document);
      changed = true;
    }
  }

  if (ImGui::CollapsingHeader("初期トランスフォーム",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    const Math::TransformLH transformBefore = document.initialTransform;
    document.initialTransform.ImGui("初期トランスフォーム");
    const Math::TransformLH &transformAfter = document.initialTransform;
    if (transformBefore.translate.x != transformAfter.translate.x ||
        transformBefore.translate.y != transformAfter.translate.y ||
        transformBefore.translate.z != transformAfter.translate.z ||
        transformBefore.scale.x != transformAfter.scale.x ||
        transformBefore.scale.y != transformAfter.scale.y ||
        transformBefore.scale.z != transformAfter.scale.z ||
        transformBefore.quaternion.x != transformAfter.quaternion.x ||
        transformBefore.quaternion.y != transformAfter.quaternion.y ||
        transformBefore.quaternion.z != transformAfter.quaternion.z ||
        transformBefore.quaternion.w != transformAfter.quaternion.w) {
      changed = true;
    }
    bool transformClipboardChanged = false;
    if (EntityEditorInspectorTransformCopyPasteUi::Draw(
            *this, document.initialTransform, transformClipboardChanged)) {
      changed |= transformClipboardChanged;
    }
  }

  ImGui::Separator();
  ImGui::TextUnformatted("コンポーネント");

  static int addPresetListIndex = 0;

  const char *kindNames[] = {"Upd", "Draw", "Data"};
  int kindComboIndex =
      ComponentSlotKindToKindComboIndex(ui_.selectedComponentKind);
  if (ImGui::Combo("種別 (Upd/Draw/Data)", &kindComboIndex, kindNames,
                   IM_ARRAYSIZE(kindNames))) {
    ui_.selectedComponentKind =
        ComponentSlotKindFromKindComboIndex(kindComboIndex);
    SyncSelectedComponentToKind(ui_, document);
    addPresetListIndex = 0;
  }

  std::vector<int> presetIndicesForKind;
  CollectKnownPresetIndicesForKind(ui_.selectedComponentKind,
                                   presetIndicesForKind);
  if (!presetIndicesForKind.empty()) {
    addPresetListIndex = std::clamp(
        addPresetListIndex, 0,
        static_cast<int>(presetIndicesForKind.size()) - 1);

    std::vector<std::string> presetLabelsStorage;
    presetLabelsStorage.reserve(presetIndicesForKind.size());
    std::vector<const char *> presetLabels;
    presetLabels.reserve(presetIndicesForKind.size());
    for (const int presetIndex : presetIndicesForKind) {
      presetLabelsStorage.emplace_back(kKnownComponents[presetIndex].typeName);
      presetLabels.push_back(presetLabelsStorage.back().c_str());
    }
    ImGui::Combo("追加する型", &addPresetListIndex, presetLabels.data(),
                 static_cast<int>(presetLabels.size()));
    if (ImGui::Button("コンポーネントを追加")) {
      const KnownComponentEntry &entry =
          kKnownComponents[presetIndicesForKind[addPresetListIndex]];
      AppendKnownComponentSlot(document, entry);
      ui_.selectedComponentIndex =
          static_cast<int>(document.components.size()) - 1;
      EntityEditorAuthoringDocumentEditNotify::Notify(*this);
    }
  } else {
    ImGui::TextDisabled(
        "この種別では追加できるプリセットがありません");
  }

  EntityEditorInspectorImportedDropBatchApplyUi::Draw(*this);

  const bool changedBeforeComponentInspector = changed;
  SyncSelectedComponentToKind(ui_, document);
  std::vector<int> kindComponentIndices =
      CollectComponentIndicesForKind(document, ui_.selectedComponentKind);

  if (kindComponentIndices.empty()) {
    ImGui::TextUnformatted(
        "選択中の種別にコンポーネントがありません。「コンポーネントを追加」で追加してください。");
  } else {
    std::sort(kindComponentIndices.begin(), kindComponentIndices.end(),
              [&document](const int a, const int b) {
                return document.components[static_cast<size_t>(a)]
                           .executionOrder <
                       document.components[static_cast<size_t>(b)]
                           .executionOrder;
              });

    ImGui::TextUnformatted("実行順 | 型");
    for (int listIndex = 0;
         listIndex < static_cast<int>(kindComponentIndices.size()); ++listIndex) {
      const int docIndex = kindComponentIndices[static_cast<size_t>(listIndex)];
      const ComponentSlot &slot =
          document.components[static_cast<size_t>(docIndex)];
      const std::string label = std::to_string(slot.executionOrder) + " | " +
                                slot.typeName;
      if (ImGui::Selectable(label.c_str(),
                            ui_.selectedComponentIndex == docIndex)) {
        ui_.selectedComponentIndex = docIndex;
      }
    }

    ImGui::Separator();
    const ComponentSlot &slotForRemove =
        document.components[static_cast<size_t>(ui_.selectedComponentIndex)];
    bool componentRemoveChanged = false;
    EntityEditorInspectorComponentRemoveBatchUi::Draw(*this, document,
                                                      slotForRemove,
                                                      componentRemoveChanged);
    if (componentRemoveChanged) {
      SyncSelectedComponentToKind(ui_, document);
      changed = true;
      kindComponentIndices =
          CollectComponentIndicesForKind(document, ui_.selectedComponentKind);
    }

    if (kindComponentIndices.empty() ||
        ui_.selectedComponentIndex < 0 ||
        ui_.selectedComponentIndex >=
            static_cast<int>(document.components.size())) {
      ImGui::TextDisabled("コンポーネントはすべて削除されました");
    } else {
    ComponentSlot &selected =
        document.components[static_cast<size_t>(ui_.selectedComponentIndex)];

    if (DrawComponentSettingsCopyPasteButtons(*this, selected)) {
      EntityEditorAuthoringDocumentEditNotify::NotifyPreviewOnly(*this);
    }

    bool changedInComponentSectionBesidesAssetPath = false;
    bool executionOrderChanged = false;
    EntityEditorInspectorExecutionOrderBatchUi::Draw(*this, document, selected,
                                                     executionOrderChanged);
    if (executionOrderChanged) {
      changed = true;
      changedInComponentSectionBesidesAssetPath = true;
    }

    if (!presetIndicesForKind.empty()) {
      int typeComboListIndex = 0;
      for (int i = 0; i < static_cast<int>(presetIndicesForKind.size()); ++i) {
        if (selected.typeName ==
            kKnownComponents[presetIndicesForKind[static_cast<size_t>(i)]]
                .typeName) {
          typeComboListIndex = i;
          break;
        }
      }
      std::vector<std::string> typeLabelStorage;
      typeLabelStorage.reserve(presetIndicesForKind.size());
      std::vector<const char *> typeLabels;
      typeLabels.reserve(presetIndicesForKind.size());
      for (const int presetIndex : presetIndicesForKind) {
        typeLabelStorage.emplace_back(kKnownComponents[presetIndex].typeName);
        typeLabels.push_back(typeLabelStorage.back().c_str());
      }
      if (ImGui::Combo("コンポーネント型", &typeComboListIndex, typeLabels.data(),
                       static_cast<int>(typeLabels.size()))) {
        const KnownComponentEntry &entry =
            kKnownComponents[presetIndicesForKind[static_cast<size_t>(
                typeComboListIndex)]];
        EntityEditorInspectorComponentTypeComboApply::
            ApplyKnownComponentTypeFromCombo(selected, ui_.selectedComponentKind,
                                             entry.typeName);
        changed = true;
        changedInComponentSectionBesidesAssetPath = true;
      }
    } else {
      ImGui::TextUnformatted(selected.typeName.c_str());
    }

    bool editedCmpEditorAssetPathParams = false;
    if (DrawComponentParams(selected, ui_)) {
      changed = true;
      if (selected.typeName == "CmpEditorAssetPath") {
        editedCmpEditorAssetPathParams = true;
      }
    }

    if (changed) {
      const bool assetPathParamsOnly =
          editedCmpEditorAssetPathParams && !changedBeforeComponentInspector &&
          !changedInComponentSectionBesidesAssetPath;
      if (assetPathParamsOnly) {
        EntityEditorAuthoringEditorAssetPathEditNotify::
            NotifyEditorAssetPathPreviewRefresh(*this);
      } else {
        EntityEditorAuthoringDocumentEditNotify::Notify(*this);
      }
    }
    }
  } else if (changed) {
    EntityEditorAuthoringDocumentEditNotify::Notify(*this);
  }
}

void EntityEditor::DrawTagsPanel() {
  bool changedScene = false;

  ImGui::InputString("検索", ui_.tagSearchFilter);
  ImGui::Separator();

  ImGui::InputString("新規タグ名", ui_.newCatalogTagName);
  ImGui::SameLine();
  if (ImGui::Button("タグを追加")) {
    if (AddTagToCatalog(scene_, ui_.newCatalogTagName)) {
      ui_.selectedCatalogTag = ui_.newCatalogTagName;
      ui_.renameCatalogTagName = ui_.newCatalogTagName;
      ui_.newCatalogTagName.clear();
      changedScene = true;
    }
  }

  const std::vector<std::string> visibleTags =
      FilterTagCatalog(scene_, ui_.tagSearchFilter);

  ImGui::Separator();
  ImGui::TextDisabled("登録タグ (%zu)", visibleTags.size());

  if (visibleTags.empty()) {
    ImGui::TextUnformatted("タグがありません");
  } else {
    for (const std::string &tagName : visibleTags) {
      const bool selected = ui_.selectedCatalogTag == tagName;
      if (ImGui::Selectable(tagName.c_str(), selected)) {
        ui_.selectedCatalogTag = tagName;
        ui_.renameCatalogTagName = tagName;
      }
    }
  }

  if (!ui_.selectedCatalogTag.empty()) {
    ImGui::Separator();
    ImGui::Text("選択: %s", ui_.selectedCatalogTag.c_str());
    ImGui::Text("Hash64: 0x%016llX",
                static_cast<unsigned long long>(Hash64(ui_.selectedCatalogTag)));

    ImGui::InputString("名前を変更", ui_.renameCatalogTagName);
    ImGui::SameLine();
    if (ImGui::Button("改名を適用")) {
      const std::string fromName = ui_.selectedCatalogTag;
      if (RenameTagInDocument(scene_, fromName, ui_.renameCatalogTagName)) {
        ui_.selectedCatalogTag = ui_.renameCatalogTagName;
        changedScene = true;
      }
    }

    ImGui::SameLine();
    if (ImGui::Button("タグを削除")) {
      RemoveTagFromDocument(scene_, ui_.selectedCatalogTag);
      ui_.selectedCatalogTag.clear();
      ui_.renameCatalogTagName.clear();
      changedScene = true;
    }
  }

  if (changedScene) {
    EntityEditorAuthoringDocumentEditNotify::Notify(*this);
  }
}

} // namespace Atrum::EntityEditor

#endif
