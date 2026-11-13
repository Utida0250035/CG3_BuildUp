#include "System/Entity/Components/CmpBillboardInstancedGrid.h"

#include "Camera/ActiveCameraSystem.h"
#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Renderer/BillboardMatrix.h"
#include "Math/Transform.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

void CmpBillboardInstancedGrid::RebuildInstanceLayout() {
  localOffsets_.clear();
  const int countX = std::max(1, gridCountX_);
  const int countY = std::max(1, gridCountY_);
  localOffsets_.reserve(static_cast<size_t>(countX * countY));

  const float originX = -0.5f * static_cast<float>(countX - 1) * spacing_;
  const float originZ = -0.5f * static_cast<float>(countY - 1) * spacing_;
  for (int y = 0; y < countY; ++y) {
    for (int x = 0; x < countX; ++x) {
      localOffsets_.push_back(
          {originX + static_cast<float>(x) * spacing_, 0.0f,
           originZ + static_cast<float>(y) * spacing_});
    }
  }
}

void CmpBillboardInstancedGrid::Initialize() {
  RebuildInstanceLayout();
  gpuBatch_.resize(localOffsets_.size());

  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine != nullptr && !textureFilePath_.empty()) {
    textureIndex_ = engine->GetTexture(textureFilePath_);
  }
  if (textureIndex_ == 0) {
    textureIndex_ = WhiteTextureIndex();
  }
}

void CmpBillboardInstancedGrid::Draw() {
  if (localOffsets_.empty()) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine == nullptr) {
    return;
  }

  Camera *camera = ActiveCameraSystem::Instance().GetActiveCamera();
  if (camera == nullptr) {
    return;
  }

  camera->UpdateMatrix();
  const Math::Matrix4x4 viewMatrix = camera->GetViewMatrix();
  const Math::Vector3 ownerWorld = RefOwner().GetWorldPosition();
  const Math::Vector3 scale{billboardWidth_, billboardHeight_, 1.0f};

  if (gpuBatch_.size() != localOffsets_.size()) {
    gpuBatch_.resize(localOffsets_.size());
  }

  for (size_t i = 0; i < localOffsets_.size(); ++i) {
    const Math::Vector3 worldPos = ownerWorld + localOffsets_[i];
    ParticleInstanceGpuData &gpu = gpuBatch_[i];
    gpu.radius = 1.0f;
    gpu.world = Renderer::MakeViewPlaneParallelBillboardWorld(
        worldPos, scale, viewMatrix);

    const float tint =
        0.75f + 0.25f * std::sin(static_cast<float>(i) * 0.37f);
    gpu.color = {baseColor_.x * tint, baseColor_.y * tint, baseColor_.z * tint,
                 baseColor_.w};
  }

  engine->DrawBillboardParticlesInstanced(textureIndex_, Math::Transform{},
                                        gpuBatch_, false);
}

} // namespace Atrum
