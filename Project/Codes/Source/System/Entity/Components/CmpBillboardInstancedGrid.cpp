#include "System/Entity/Components/CmpBillboardInstancedGrid.h"

#include "Camera/ActiveCameraSystem.h"
#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Renderer/BillboardMatrix.h"
#include "Math/Transform.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

float CmpBillboardInstancedGrid::ResolvedBillboardWidth() const {
  if (useAutoSizeInCell_) {
    return cellPitch_ * cellFill_;
  }
  return billboardWidth_;
}

float CmpBillboardInstancedGrid::ResolvedBillboardHeight() const {
  if (useAutoSizeInCell_) {
    return cellPitch_ * cellFill_;
  }
  return billboardHeight_;
}

void CmpBillboardInstancedGrid::RebuildInstanceLayout() {
  localOffsets_.clear();
  const int countX = std::max(1, gridCountX_);
  const int countY = std::max(1, gridCountY_);
  localOffsets_.reserve(static_cast<size_t>(countX * countY));

  // マス目: セル (ix, iy) の中心を cellPitch 間隔で格子配置
  const float originX = -0.5f * static_cast<float>(countX - 1) * cellPitch_;
  const float originZ = -0.5f * static_cast<float>(countY - 1) * cellPitch_;
  for (int iy = 0; iy < countY; ++iy) {
    for (int ix = 0; ix < countX; ++ix) {
      localOffsets_.push_back(
          {originX + static_cast<float>(ix) * cellPitch_, 0.0f,
           originZ + static_cast<float>(iy) * cellPitch_});
    }
  }
}

void CmpBillboardInstancedGrid::DrawCellFloorGrid(
    AtrumEngine *engine, const Math::Vector3 &origin) const {
  if (!drawCellGridLines_ || engine == nullptr) {
    return;
  }

  const int countX = std::max(1, gridCountX_);
  const int countY = std::max(1, gridCountY_);
  const float halfCell = 0.5f * cellPitch_;
  const float minX =
      origin.x - 0.5f * static_cast<float>(countX - 1) * cellPitch_ - halfCell;
  const float maxX =
      origin.x + 0.5f * static_cast<float>(countX - 1) * cellPitch_ + halfCell;
  const float minZ =
      origin.z - 0.5f * static_cast<float>(countY - 1) * cellPitch_ - halfCell;
  const float maxZ =
      origin.z + 0.5f * static_cast<float>(countY - 1) * cellPitch_ + halfCell;
  const float y = origin.y + gridPlaneYOffset_;

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  const float lineWidth = std::max(0.008f, cellPitch_ * 0.035f);

  for (int ix = 0; ix <= countX; ++ix) {
    const float x = minX + static_cast<float>(ix) * cellPitch_;
    engine->DrawLine3D(WhiteTextureIndex(), gridLineColor_, Math::Transform{},
                     {x, y, minZ}, {x, y, maxZ}, lineWidth);
  }
  for (int iz = 0; iz <= countY; ++iz) {
    const float z = minZ + static_cast<float>(iz) * cellPitch_;
    engine->DrawLine3D(WhiteTextureIndex(), gridLineColor_, Math::Transform{},
                     {minX, y, z}, {maxX, y, z}, lineWidth);
  }
  engine->SetBlendModeForFrame(BlendMode::NONE);
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

  const int countX = std::max(1, gridCountX_);
  const Math::Vector3 ownerWorld = RefOwner().GetWorldPosition();
  DrawCellFloorGrid(engine, ownerWorld);

  camera->UpdateMatrix();
  const Math::Matrix4x4 viewMatrix = camera->GetViewMatrix();
  const float drawWidth = ResolvedBillboardWidth();
  const float drawHeight = ResolvedBillboardHeight();
  const Math::Vector3 scale{drawWidth, drawHeight, 1.0f};

  if (gpuBatch_.size() != localOffsets_.size()) {
    gpuBatch_.resize(localOffsets_.size());
  }

  for (size_t i = 0; i < localOffsets_.size(); ++i) {
    const int ix = static_cast<int>(i) % countX;
    const int iy = static_cast<int>(i) / countX;
    const Math::Vector3 worldPos = ownerWorld + localOffsets_[i];

    ParticleInstanceGpuData &gpu = gpuBatch_[i];
    gpu.radius = 1.0f;
    gpu.world = Renderer::MakeViewPlaneParallelBillboardWorld(
        worldPos, scale, viewMatrix);

    if (checkerboardEnabled_) {
      const bool isSecondary = (ix + iy) % 2 == 1;
      gpu.color = isSecondary ? checkerColorB_ : checkerColorA_;
    } else {
      gpu.color = baseColor_;
    }
  }

  engine->DrawBillboardParticlesInstanced(textureIndex_, Math::Transform{},
                                        gpuBatch_, false);
}

} // namespace Atrum
