#include "Scene/SceneGameLightingProfile.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Resource/LightData.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/GameAssetsPaths.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace {

using Atrum::DirectionalLightData;
using Atrum::Engine;
using Atrum::LightModel;
using Atrum::Math::Vector3;

bool TryReadVector3(const nlohmann::json &node, const char *key, Vector3 &out) {
  if (!node.contains(key) || !node[key].is_array() || node[key].size() < 3) {
    return false;
  }
  out.x = node[key][0].get<float>();
  out.y = node[key][1].get<float>();
  out.z = node[key][2].get<float>();
  return true;
}

LightModel ParseLightModel(const nlohmann::json &node, const char *key) {
  if (!node.contains(key)) {
    return LightModel::HalfLambert;
  }
  if (node[key].is_string()) {
    const std::string name = node[key].get<std::string>();
    if (name == "Lambert") {
      return LightModel::Lambert;
    }
  }
  return LightModel::HalfLambert;
}

DirectionalLightData BuildDefaultDirectional() {
  Vector3 lightDirection{-0.4f, -0.85f, 0.15f};
  lightDirection.Normalize();
  return DirectionalLightData{
      .color = Vector3{1.0f, 1.0f, 1.0f},
      .direction = lightDirection,
      .intensity = 8.0f,
      .lightModel = LightModel::HalfLambert,
      .rimColor = Vector3{0.75f, 0.85f, 1.0f},
      .rimIntensity = 0.35f,
      .rimPower = 3.0f,
      .rimEnabled = 1u,
  };
}

bool TryLoadJsonFile(const std::string &path, nlohmann::json &out) {
  namespace fs = std::filesystem;
  std::error_code error;
  if (!fs::exists(path, error)) {
    return false;
  }
  std::ifstream stream(path);
  if (!stream.is_open()) {
    return false;
  }
  try {
    stream >> out;
  } catch (const nlohmann::json::exception &) {
    return false;
  }
  return out.is_object();
}

bool TryResolveProfileJson(nlohmann::json &out) {
  if (TryLoadJsonFile(Atrum::GameAssets::kGameSceneLightingProfileJson, out)) {
    return true;
  }
  return TryLoadJsonFile(Atrum::EditorAssets::kGameSceneLightingProfileJson,
                         out);
}

void MergeDirectionalFromJson(const nlohmann::json &root,
                              DirectionalLightData &data) {
  if (!root.contains("directionalLight") ||
      !root["directionalLight"].is_object()) {
    return;
  }
  const nlohmann::json &node = root["directionalLight"];
  TryReadVector3(node, "color", data.color);
  if (TryReadVector3(node, "direction", data.direction)) {
    data.direction.Normalize();
  }
  if (node.contains("intensity")) {
    data.intensity = node["intensity"].get<float>();
  }
  data.lightModel = ParseLightModel(node, "lightModel");
  TryReadVector3(node, "rimColor", data.rimColor);
  if (node.contains("rimIntensity")) {
    data.rimIntensity = node["rimIntensity"].get<float>();
  }
  if (node.contains("rimPower")) {
    data.rimPower = node["rimPower"].get<float>();
  }
  if (node.contains("rimEnabled")) {
    data.rimEnabled = node["rimEnabled"].get<uint32_t>();
  }
}

void PushDirectionalToEngine(const DirectionalLightData &directional) {
  Engine *const engine = Engine::Instance();
  if (engine == nullptr) {
    return;
  }
  ::Atrum::EngineSetDirectionalLightData(directional);
  engine->FlushDirectionalLightConstantBuffer();
}

} // namespace

bool ApplySceneGameLightingFromProfile() {
  DirectionalLightData directional = BuildDefaultDirectional();

  nlohmann::json root;
  if (TryResolveProfileJson(root)) {
    MergeDirectionalFromJson(root, directional);
  }

  PushDirectionalToEngine(directional);
  return true;
}