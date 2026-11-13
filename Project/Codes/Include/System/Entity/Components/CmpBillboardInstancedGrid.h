#pragma once

#include "Engine/Resource/ParticleGpuData.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"
#include <cstdint>
#include <string>
#include <vector>

namespace Atrum {

/// グリッド状に並べたビルボードを GPU インスタンシングで一括描画
class CmpBillboardInstancedGrid : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void Initialize() override;
  void Draw() override;

  void SetTextureFilePath(const std::string &path) { textureFilePath_ = path; }
  void SetGridCountX(const int count) { gridCountX_ = std::max(1, count); }
  void SetGridCountY(const int count) { gridCountY_ = std::max(1, count); }
  void SetSpacing(const float spacing) { spacing_ = spacing; }
  void SetBillboardWidth(const float width) { billboardWidth_ = width; }
  void SetBillboardHeight(const float height) { billboardHeight_ = height; }
  void SetBaseColor(const Math::Vector4 &color) { baseColor_ = color; }

private:
  void RebuildInstanceLayout();

  std::string textureFilePath_{"./Assets/Images/white4x4.png"};
  int gridCountX_ = 10;
  int gridCountY_ = 10;
  float spacing_ = 1.15f;
  float billboardWidth_ = 0.85f;
  float billboardHeight_ = 0.85f;
  Math::Vector4 baseColor_{1.0f, 1.0f, 1.0f, 1.0f};

  uint32_t textureIndex_ = 0;
  std::vector<Math::Vector3> localOffsets_{};
  std::vector<ParticleInstanceGpuData> gpuBatch_{};
};

} // namespace Atrum
