#include "Scene/SceneGameLightingProfile.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Resource/LightData.h"
#include "Engine/Renderer/Lighting/DirectionalShadow.h"
#include "Engine/Renderer/Lighting/GodRays.h"
#include "Engine/Renderer/Lighting/VolumetricFog.h"
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
using Atrum::Visual::DirectionalShadowSettings;
using Atrum::Visual::GodRaySettings;
using Atrum::Visual::VolumetricFogSettings;

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

VolumetricFogSettings BuildDefaultFog() {
  VolumetricFogSettings fog{};
  fog.enabled = 1u;
  fog.density = 0.022f;
  fog.heightFalloff = 0.28f;
  fog.heightOffset = 0.0f;
  fog.maxDistance = 90.0f;
  fog.inScatterIntensity = 0.18f;
  return fog;
}

GodRaySettings BuildDefaultGodRays() {
  GodRaySettings rays{};
  rays.enabled = 1u;
  rays.intensity = 0.12f;
  rays.angularPower = 8.0f;
  rays.fogCoupling = 0.9f;
  return rays;
}

DirectionalShadowSettings BuildDefaultShadow() {
  DirectionalShadowSettings shadow{};
  shadow.shadeEnabled = 1u;
  shadow.shadeDarkness = 0.42f;
  shadow.groundPlaneY = 0.0f;
  shadow.contactHeight = 3.0f;
  shadow.planarEnabled = 1u;
  shadow.planarAlpha = 0.32f;
  return shadow;
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

void MergeFogFromJson(const nlohmann::json &root, VolumetricFogSettings &fog) {
  if (!root.contains("volumetricFog") || !root["volumetricFog"].is_object()) {
    return;
  }
  const nlohmann::json &node = root["volumetricFog"];
  TryReadVector3(node, "color", fog.color);
  if (node.contains("enabled")) {
    fog.enabled = node["enabled"].get<uint32_t>();
  }
  if (node.contains("density")) {
    fog.density = node["density"].get<float>();
  }
  if (node.contains("heightFalloff")) {
    fog.heightFalloff = node["heightFalloff"].get<float>();
  }
  if (node.contains("heightOffset")) {
    fog.heightOffset = node["heightOffset"].get<float>();
  }
  if (node.contains("maxDistance")) {
    fog.maxDistance = node["maxDistance"].get<float>();
  }
  if (node.contains("inScatterIntensity")) {
    fog.inScatterIntensity = node["inScatterIntensity"].get<float>();
  }
}

void MergeGodRaysFromJson(const nlohmann::json &root, GodRaySettings &rays) {
  if (!root.contains("godRays") || !root["godRays"].is_object()) {
    return;
  }
  const nlohmann::json &node = root["godRays"];
  TryReadVector3(node, "color", rays.color);
  if (node.contains("enabled")) {
    rays.enabled = node["enabled"].get<uint32_t>();
  }
  if (node.contains("intensity")) {
    rays.intensity = node["intensity"].get<float>();
  }
  if (node.contains("angularPower")) {
    rays.angularPower = node["angularPower"].get<float>();
  }
  if (node.contains("fogCoupling")) {
    rays.fogCoupling = node["fogCoupling"].get<float>();
  }
}

void MergeShadowFromJson(const nlohmann::json &root,
                         DirectionalShadowSettings &shadow) {
  if (!root.contains("directionalShadow") ||
      !root["directionalShadow"].is_object()) {
    return;
  }
  const nlohmann::json &node = root["directionalShadow"];
  if (node.contains("shadeEnabled")) {
    shadow.shadeEnabled = node["shadeEnabled"].get<uint32_t>();
  }
  if (node.contains("shadeDarkness")) {
    shadow.shadeDarkness = node["shadeDarkness"].get<float>();
  }
  if (node.contains("groundPlaneY")) {
    shadow.groundPlaneY = node["groundPlaneY"].get<float>();
  }
  if (node.contains("contactHeight")) {
    shadow.contactHeight = node["contactHeight"].get<float>();
  }
  if (node.contains("planarEnabled")) {
    shadow.planarEnabled = node["planarEnabled"].get<uint32_t>();
  }
  if (node.contains("planarAlpha")) {
    shadow.planarAlpha = node["planarAlpha"].get<float>();
  }
}

void PushToEngine(const DirectionalLightData &directional,
                  const VolumetricFogSettings &fog, const GodRaySettings &rays,
                  const DirectionalShadowSettings &shadow) {
  Engine *const engine = Engine::Instance();
  if (engine == nullptr) {
    return;
  }
  // Fog / GodRays / Shadow を先に格納してから SetDirectionalLightData→Flush へ渡す
  engine->RefVolumetricFog().SetSettings(fog);
  engine->RefGodRays().SetSettings(rays);
  engine->RefDirectionalShadow().SetSettings(shadow);
  ::Atrum::EngineSetDirectionalLightData(directional);
  engine->FlushDirectionalLightConstantBuffer();
}

} // namespace

bool ApplySceneGameLightingFromProfile() {
  DirectionalLightData directional = BuildDefaultDirectional();
  VolumetricFogSettings fog = BuildDefaultFog();
  GodRaySettings rays = BuildDefaultGodRays();
  DirectionalShadowSettings shadow = BuildDefaultShadow();

  nlohmann::json root;
  if (TryResolveProfileJson(root)) {
    MergeDirectionalFromJson(root, directional);
    MergeFogFromJson(root, fog);
    MergeGodRaysFromJson(root, rays);
    MergeShadowFromJson(root, shadow);
  }

  PushToEngine(directional, fog, rays, shadow);
  return true;
}