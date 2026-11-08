#include "Engine/Renderer/Lighting/Lighting.h"
#include "Engine/Command/CommandContext.h"

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Visual {

void Lighting::Initialize(ID3D12Device *device) {
  lightingBuffer_ = std::make_unique<SingleConstantBuffer<SceneLightData>>();
  lightingBuffer_->CreateBuffer(device);

  sceneLightData_.directionalLight.color = Math::Vector3::One();
  sceneLightData_.directionalLight.intensity = 1.0f;
  sceneLightData_.directionalLight.direction = Math::Vector3::DownLh();
  sceneLightData_.directionalLight.lightModel = LightModel::HalfLambert;
  volumetricFog_.RefSettings().enabled = 0u;
}

void Lighting::Update(CommandContext *commandContext, UINT rootParameterIndex) {
  if (!lightingBuffer_ || !commandContext) {
    return;
  }

  volumetricFog_.ApplyTo(sceneLightData_.directionalLight);
  godRays_.ApplyTo(sceneLightData_.directionalLight);
  lightingBuffer_->SetData(sceneLightData_);

  auto commandList = commandContext->GetCommandList();

  if (commandList) {
    commandList->SetGraphicsRootConstantBufferView(
        rootParameterIndex, lightingBuffer_->GetGpuVirtualAddress());
  }
}

#ifdef USE_IMGUI

void Lighting::ImGui() {
  if (ImGui::CollapsingHeader("VolumetricFog", ImGuiTreeNodeFlags_DefaultOpen)) {
    volumetricFog_.ImGui();
  }
  if (ImGui::CollapsingHeader("GodRays", ImGuiTreeNodeFlags_DefaultOpen)) {
    godRays_.ImGui();
  }
}

#endif

} // namespace Atrum::Visual