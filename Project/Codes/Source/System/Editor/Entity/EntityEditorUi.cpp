#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Entity/EntityEditor.h"

#include "System/Editor/EditorEntityWorkbench.h"
#include "System/Editor/Prefab/PrefabAsset.h"
#include "System/Editor/Entity/EntityEditorTagCatalog.h"
#include "System/Editor/EditorImportedAsset.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/Hash64.h"
#include "ForDebug/ImGui.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/EntityTagRegistry.h"

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
    {"CmpDrawHitSphere", ComponentSlotKind::Draw},
    {"CmpEditorAssetPath", ComponentSlotKind::Data},
    {"CmpRotator", ComponentSlotKind::Upd},
    {"CmpScroll", ComponentSlotKind::Upd},
    {"CmpHitSphere", ComponentSlotKind::Upd},
};

void AppendKnownComponentSlot(EntityDefinition &definition,
                              const KnownComponentEntry &entry) {
  ComponentSlot slot{};
  slot.kind = entry.kind;
  slot.typeName = entry.typeName;
  slot.params = DefaultParamsForType(entry.typeName);
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

bool DrawComponentParams(ComponentSlot &slot, const UiState &ui) {
  nlohmann::json &p = slot.params;
  bool changed = false;

  if (slot.typeName == "CmpEditorAssetPath") {
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

void DrawHierarchyNode(EntityEditor &editor, const int nodeIndex) {
  EditorSceneDocument &scene = editor.SceneDocument();
  UiState &ui = editor.Ui();
  if (nodeIndex < 0 ||
      nodeIndex >= static_cast<int>(scene.instances.size())) {
    return;
  }
  SceneEntityInstance &instance =
      scene.instances[static_cast<size_t>(nodeIndex)];
  const bool selected = ui.selectedInstanceIndex == nodeIndex;
  ImGui::PushID(nodeIndex);
  if (ImGui::Selectable(instance.instanceName.c_str(), selected)) {
    ui.selectedInstanceIndex = nodeIndex;
  }
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
  if (ui_.interactionMode == EditorInteractionMode::Play) {
    ImGui::TextDisabled("モード: プレイ");
  } else {
    ImGui::TextDisabled("モード: エディット");
  }

  ImGui::Separator();
  ImGui::InputString("シーン名", scene_.sceneName);

  if (ImGui::Button("オブジェクトを追加")) {
    AddEmptyInstance();
  }
  ImGui::SameLine();
  if (scene_.instances.size() > 1 && ImGui::Button("削除")) {
    RemoveSelectedInstance();
  }

  ImGui::Separator();
  if (ImGui::TreeNodeEx(scene_.sceneName.c_str(),
                        ImGuiTreeNodeFlags_DefaultOpen)) {
    for (int i = 0; i < static_cast<int>(scene_.instances.size()); ++i) {
      if (scene_.instances[static_cast<size_t>(i)].parentIndex < 0) {
        DrawHierarchyNode(*this, i);
      }
    }
    ImGui::TreePop();
  }
}

void EntityEditor::DrawProjectPanel() {
  Atrum::Editor::Scene::EnsureScenesDirectoryExists();

  ImGui::TextUnformatted("シーン一覧");
  if (ImGui::Button("新規シーン")) {
    CreateNewSceneFile();
  }
  ImGui::SameLine();
  if (ImGui::Button("シーンを実行")) {
    ui_.requestSaveToDisk = true;
    ui_.requestPlayRuntimeScene = true;
  }
  ImGui::Separator();

  ImGui::TextUnformatted("ファイルドロップ（obj / png 等）");
  ImGui::TextDisabled("ウィンドウへドロップ → %s",
                      Atrum::Editor::Import::kImportedAssetsDirectory);
  if (!ui_.lastImportedAssetPath.empty()) {
    ImGui::TextWrapped("直近: %s", ui_.lastImportedAssetPath.c_str());
  }
  if (!ui_.importStatusMessage.empty()) {
    ImGui::TextWrapped("%s", ui_.importStatusMessage.c_str());
  }
  ImGui::Separator();

  const std::vector<std::string> scenePaths =
      Atrum::Editor::Scene::ListSceneJsonFiles();
  if (scenePaths.empty()) {
    ImGui::TextDisabled("シーンがありません: %s",
                        Atrum::Editor::Scene::kScenesDirectory);
  } else {
    for (const std::string &path : scenePaths) {
      const bool isActive =
          !ui_.selectedScenePath.empty()
              ? ui_.selectedScenePath == path
              : ui_.jsonFilePath == path;
      const std::string label =
          Atrum::Editor::Scene::SceneDisplayNameFromPath(path) + "  |  " +
          path;
      if (ImGui::Selectable(label.c_str(), isActive)) {
        ui_.selectedScenePath = path;
      }
    }
  }

  ImGui::Separator();
  if (!ui_.selectedScenePath.empty()) {
    if (ImGui::Button("シーンを開く")) {
      OpenSceneFromPath(ui_.selectedScenePath);
    }
    ImGui::SameLine();
    if (ImGui::Button("シーンを削除")) {
      QueueDeleteSceneConfirm(ui_, ui_.selectedScenePath);
    }
  }

  DrawDeleteSceneConfirmPopup(*this);

  ImGui::Spacing();
  ImGui::TextUnformatted("プレハブ一覧 (Entity JSON)");
  ImGui::Separator();

  if (!scene_.instances.empty() &&
      ImGui::Button("選択エンティティをプレハブ JSON へ出力")) {
    Atrum::Editor::Prefab::EnsureEntitiesPrefabDirectoryExists();
    const std::string path = Atrum::Editor::Prefab::MakeUniquePrefabFilePath(
        SelectedDefinition().entityName);
    if (!path.empty()) {
      SaveSelectedInstanceToPrefabPath(path);
      ui_.selectedPrefabPath = path;
    }
  }
  ImGui::Spacing();

  const std::vector<std::string> prefabPaths =
      Atrum::Editor::Prefab::ListPrefabJsonFiles();
  if (prefabPaths.empty()) {
    ImGui::TextDisabled("プレハブが見つかりません: %s",
                        Atrum::Editor::Prefab::kEntitiesPrefabDirectory);
    return;
  }

  for (const std::string &path : prefabPaths) {
    const bool isActive = ui_.selectedPrefabPath == path;
    if (ImGui::Selectable(path.c_str(), isActive)) {
      ui_.selectedPrefabPath = path;
    }
  }

  ImGui::Separator();
  if (!ui_.selectedPrefabPath.empty() &&
      ImGui::Button("シーンへ追加")) {
    AddInstanceFromPrefabPath(ui_.selectedPrefabPath);
  }
}

void EntityEditor::DrawInspectorPanel() {
  if (scene_.instances.empty()) {
    ImGui::TextUnformatted("シーンにオブジェクトがありません");
    return;
  }

  bool changed = false;
  SceneEntityInstance &instance = SelectedInstance();
  EntityDefinition &document = SelectedDefinition();

  ImGui::InputString("シーン JSON パス", ui_.jsonFilePath);
  changed |= ImGui::InputString("表示名", instance.instanceName);

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
  changed |= ImGui::InputString("エンティティ名", document.entityName);

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
      document.tags.clear();
      document.tagHashes.clear();
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
      ui_.requestApplyPreview = true;
    }
  } else {
    ImGui::TextDisabled(
        "この種別では追加できるプリセットがありません");
  }

  SyncSelectedComponentToKind(ui_, document);
  std::vector<int> kindComponentIndices =
      CollectComponentIndicesForKind(document, ui_.selectedComponentKind);

  if (!kindComponentIndices.empty() &&
      ImGui::Button("選択中のコンポーネントを削除")) {
    document.components.erase(
        document.components.begin() + ui_.selectedComponentIndex);
    SyncSelectedComponentToKind(ui_, document);
    ui_.requestApplyPreview = true;
  }

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
    ComponentSlot &selected =
        document.components[static_cast<size_t>(ui_.selectedComponentIndex)];

    int execOrder = selected.executionOrder;
    if (ImGui::InputInt("実行順 (priority)", &execOrder)) {
      selected.executionOrder = std::max(0, execOrder);
      changed = true;
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
        selected.kind = ui_.selectedComponentKind;
        selected.typeName = entry.typeName;
        selected.params = DefaultParamsForType(entry.typeName);
        changed = true;
      }
    } else {
      ImGui::TextUnformatted(selected.typeName.c_str());
    }

    if (DrawComponentParams(selected, ui_)) {
      changed = true;
    }
  }

  if (changed) {
    ui_.requestApplyPreview = true;
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
    ui_.requestApplyPreview = true;
  }
}

} // namespace Atrum::EntityEditor

#endif
