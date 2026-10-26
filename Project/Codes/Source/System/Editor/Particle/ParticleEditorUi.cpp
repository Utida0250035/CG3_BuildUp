#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Particle/ParticleEditorUi.h"

#include "ForDebug/ImGui.h"
#include "System/Particle/ParticleParam.h"
#include "System/SimulationSpace.h"

#include <algorithm>
#include <cstdint>

namespace Atrum::ParticleEditor {

namespace {

using Atrum::SimulationSpace;
using Param = Visual::Particle::Param;
using Visual::Particle::EmitMode;
using Visual::Particle::EmitShape;
using Visual::Particle::MovementMode;
using Visual::Particle::RenderMode;

template <typename EnumT>
bool DrawEnumCombo(const char *label, EnumT &value,
                   const char *const *names, const EnumT *values,
                   const int count) {
  int currentIndex = 0;
  for (int i = 0; i < count; ++i) {
    if (values[i] == value) {
      currentIndex = i;
      break;
    }
  }
  if (ImGui::Combo(label, &currentIndex, names, count)) {
    value = values[currentIndex];
    return true;
  }
  return false;
}

bool DrawVector3(const char *label, Math::Vector3 &v, const float speed) {
  return ImGui::DragFloat3(label, &v.x, speed);
}

bool DrawVector4Color(const char *label, Math::Vector4 &color) {
  return ImGui::ColorEdit4(label, &color.x);
}

bool DrawEmitShapeFields(Param &param) {
  bool changed = false;

  switch (param.emitShape) {
  case EmitShape::POINT:
    changed |= DrawVector3("emitPointPos", param.emitPointPos, 0.02f);
    break;
  case EmitShape::SPHERE:
    changed |= DrawVector3("emitSphere.center", param.emitSphere.center, 0.02f);
    changed |= ImGui::DragFloat("emitSphere.radius", &param.emitSphere.radius,
                                0.02f, 0.0f, 256.0f);
    break;
  case EmitShape::AABB:
    changed |= DrawVector3("emitAabb.min", param.emitAabb.min, 0.02f);
    changed |= DrawVector3("emitAabb.max", param.emitAabb.max, 0.02f);
    break;
  case EmitShape::CIRCLE_XZ:
  case EmitShape::CIRCLE_XY:
  case EmitShape::CIRCLE_YZ:
    changed |= DrawVector3("emitCircle.center", param.emitCircle.center, 0.02f);
    changed |=
        ImGui::DragFloat("emitCircle.radius", &param.emitCircle.radius, 0.02f,
                         0.0f, 256.0f);
    break;
  default:
    break;
  }

  return changed;
}

bool DrawMovementFields(Param &param) {
  bool changed = false;

  changed |= DrawVector3("velocityRandom", param.velocityRandom, 0.05f);
  changed |= DrawVector3("acceleration", param.acceleration, 0.05f);

  switch (param.movementMode) {
  case MovementMode::DIRECTIONAL:
  case MovementMode::RADIAL_CONVERGE:
    changed |= DrawVector3("baseVelocity", param.baseVelocity, 0.05f);
    break;
  case MovementMode::RADIAL_SPREAD: {
    float speed = param.baseVelocity.Length();
    if (ImGui::DragFloat("baseVelocity (speed)", &speed, 0.05f, 0.0f, 512.0f)) {
      if (speed > 1e-5f) {
        param.baseVelocity = param.baseVelocity.Normalized() * speed;
      } else {
        param.baseVelocity = Math::Vector3{0.0f, speed, 0.0f};
      }
      changed = true;
    }
    ImGui::TextDisabled(
        "RADIAL_SPREAD では baseVelocity の大きさのみ速度として使われます");
    break;
  }
  default:
    break;
  }

  return changed;
}

bool DrawRenderAssetFields(Param &param) {
  bool changed = false;

  switch (param.renderMode) {
  case RenderMode::BILLBOARD:
    changed |= ImGui::InputString("textureFilePath", param.textureFilePath);
    break;
  case RenderMode::MODEL:
    changed |= ImGui::InputString("objDirectoryPath", param.objDirectoryPath);
    changed |= ImGui::InputString("objFileName", param.objFileName);
    changed |= ImGui::InputString("mtlDirectoryPath", param.mtlDirectoryPath);
    changed |= ImGui::InputString("mtlFileName", param.mtlFileName);
    break;
  case RenderMode::BUILTIN_SPHERE:
  default:
    break;
  }

  return changed;
}

bool DrawLayerParam(Param &param) {
  bool changed = false;

  if (ImGui::CollapsingHeader("モード", ImGuiTreeNodeFlags_DefaultOpen)) {
    const char *simulationNames[] = {"LOCAL", "WORLD"};
    const SimulationSpace simulationValues[] = {SimulationSpace::LOCAL,
                                                SimulationSpace::WORLD};
    changed |= DrawEnumCombo("simulationSpaceFromEmitter",
                             param.simulationSpaceFromEmitter, simulationNames,
                             simulationValues, 2);

    const char *emitModeNames[] = {"BURST", "CONTINUOUS"};
    const EmitMode emitModeValues[] = {EmitMode::BURST, EmitMode::CONTINUOUS};
    changed |= DrawEnumCombo("emitMode", param.emitMode, emitModeNames,
                             emitModeValues, 2);

    const char *emitShapeNames[] = {"POINT", "CIRCLE_XZ", "CIRCLE_XY",
                                    "CIRCLE_YZ", "AABB", "SPHERE"};
    const EmitShape emitShapeValues[] = {
        EmitShape::POINT,     EmitShape::CIRCLE_XZ, EmitShape::CIRCLE_XY,
        EmitShape::CIRCLE_YZ, EmitShape::AABB,     EmitShape::SPHERE};
    changed |= DrawEnumCombo("emitShape", param.emitShape, emitShapeNames,
                             emitShapeValues, 6);

    const char *movementNames[] = {"DIRECTIONAL", "RADIAL_SPREAD",
                                   "RADIAL_CONVERGE"};
    const MovementMode movementValues[] = {
        MovementMode::DIRECTIONAL, MovementMode::RADIAL_SPREAD,
        MovementMode::RADIAL_CONVERGE};
    changed |= DrawEnumCombo("movementMode", param.movementMode, movementNames,
                             movementValues, 3);

    const char *renderModeNames[] = {"BUILTIN_SPHERE", "MODEL", "BILLBOARD"};
    const RenderMode renderModeValues[] = {RenderMode::BUILTIN_SPHERE,
                                           RenderMode::MODEL,
                                           RenderMode::BILLBOARD};
    changed |= DrawEnumCombo("renderMode", param.renderMode, renderModeNames,
                             renderModeValues, 3);
  }

  if (ImGui::CollapsingHeader("発生", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (param.emitMode == EmitMode::BURST) {
      int burstCount = static_cast<int>(param.burstCount);
      if (ImGui::DragInt("burstCount", &burstCount, 1, 0, 4096)) {
        param.burstCount = static_cast<size_t>(std::max(burstCount, 0));
        changed = true;
      }
    } else if (param.emitMode == EmitMode::CONTINUOUS) {
      changed |=
          ImGui::DragFloat("emitRate", &param.emitRate, 0.05f, 0.01f, 512.0f);
      changed |=
          ImGui::DragFloat("duration", &param.duration, 0.01f, 0.0f, 600.0f);
      changed |= ImGui::Checkbox("isLooping", &param.isLooping);
    }

    changed |= DrawEmitShapeFields(param);

    int maxCount = static_cast<int>(param.maxCount);
    if (ImGui::DragInt("maxCount", &maxCount, 1, 1, 4096)) {
      param.maxCount = static_cast<size_t>(maxCount);
      changed = true;
    }
  }

  if (ImGui::CollapsingHeader("寿命・見た目", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= ImGui::DragFloat("minLifeDuration", &param.minLifeDuration,
                                0.01f, 0.0f, 120.0f);
    changed |= ImGui::DragFloat("maxLifeDuration", &param.maxLifeDuration,
                                0.01f, 0.0f, 120.0f);
    changed |= DrawVector3("startScale", param.startScale, 0.02f);
    changed |= DrawVector3("endScale", param.endScale, 0.02f);
    changed |= DrawVector4Color("startColor", param.startColor);
    changed |= DrawVector4Color("endColor", param.endColor);
  }

  if (ImGui::CollapsingHeader("移動", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= DrawMovementFields(param);
  }

  if (param.renderMode != RenderMode::BUILTIN_SPHERE &&
      ImGui::CollapsingHeader("描画アセット", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= DrawRenderAssetFields(param);
  }

  return changed;
}

void ClampSelectedLayer(UiState &uiState,
                        const std::vector<Param> &layerParams) {
  if (layerParams.empty()) {
    uiState.selectedLayerIndex = 0;
    return;
  }
  uiState.selectedLayerIndex = std::clamp(
      uiState.selectedLayerIndex, 0,
      static_cast<int>(layerParams.size()) - 1);
}

} // namespace

void Draw(UiState &uiState, std::vector<Param> &layerParams) {
  if (!ImGui::Begin("Particle Editor")) {
    ImGui::End();
    return;
  }

  ImGui::TextUnformatted("Space: プレビュー再生  |  R: タイトルへ戻る");
  ImGui::Separator();

  ImGui::InputString("JSON path", uiState.jsonFilePath);
  if (ImGui::Button("Reload")) {
    uiState.requestReloadFromDisk = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    uiState.requestSaveToDisk = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Apply preview")) {
    uiState.requestApplyPreview = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Replay")) {
    uiState.requestReplay = true;
  }

  ImGui::Separator();

  if (ImGui::Button("Add layer")) {
    layerParams.emplace_back();
    uiState.selectedLayerIndex =
        static_cast<int>(layerParams.size()) - 1;
    uiState.requestApplyPreview = true;
  }
  ImGui::SameLine();
  ClampSelectedLayer(uiState, layerParams);
  if (!layerParams.empty() &&
      ImGui::Button("Remove selected layer")) {
    layerParams.erase(layerParams.begin() + uiState.selectedLayerIndex);
    ClampSelectedLayer(uiState, layerParams);
    uiState.requestApplyPreview = true;
  }

  if (layerParams.empty()) {
    ImGui::TextUnformatted("レイヤーがありません。Add layer で追加してください。");
    ImGui::End();
    return;
  }

  ImGui::Separator();
  for (int i = 0; i < static_cast<int>(layerParams.size()); ++i) {
    const std::string tabLabel = "Layer " + std::to_string(i);
    if (ImGui::Selectable(tabLabel.c_str(),
                          uiState.selectedLayerIndex == i)) {
      uiState.selectedLayerIndex = i;
    }
  }

  ImGui::Separator();
  Param &selected = layerParams[static_cast<size_t>(uiState.selectedLayerIndex)];
  if (DrawLayerParam(selected)) {
    uiState.requestApplyPreview = true;
  }

  ImGui::End();
}

} // namespace Atrum::ParticleEditor

#endif
