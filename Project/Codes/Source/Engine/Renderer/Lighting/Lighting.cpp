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
}

void Lighting::Update(CommandContext *commandContext, UINT rootParameterIndex) {
  if (!lightingBuffer_ || !commandContext) {
    return;
  }

  lightingBuffer_->SetData(sceneLightData_);

  auto commandList = commandContext->GetCommandList();

  if (commandList) {
    commandList->SetGraphicsRootConstantBufferView(
        rootParameterIndex, lightingBuffer_->GetGpuVirtualAddress());
  }
}

#ifdef USE_IMGUI

void Lighting::ImGui() {}

#endif

} // namespace Atrum::Visual