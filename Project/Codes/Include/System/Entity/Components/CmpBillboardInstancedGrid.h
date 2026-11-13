#pragma once

#include "Engine/Resource/ParticleGpuData.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"
#include <cstdint>
#include <string>
#include <vector>

namespace Atrum {

class AtrumEngine;

/// グリッド状に並べたビルボードを GPU インスタンシングで一括描画（マス目整列）
class CmpBillboardInstancedGrid : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void Initialize() override;
  void Draw() override;

  void SetTextureFilePath(const std::string &path) { textureFilePath_ = path; }
  void SetGridCountX(const int count) {
    gridCountX_ = std::max(1, count);
  }
  void SetGridCountY(const int count) {
    gridCountY_ = std::max(1, count);
  }
  /// マス目の中心間隔（X/Z 共通。旧 `spacing` と同義）
  void SetSpacing(const float spacing) { cellPitch_ = spacing; }
  void SetCellPitch(const float pitch) { cellPitch_ = pitch; }
  void SetCellFill(const float fill) { cellFill_ = fill; }
  void SetUseAutoSizeInCell(const bool enabled) { useAutoSizeInCell_ = enabled; }
  void SetBillboardWidth(const float width) { billboardWidth_ = width; }
  void SetBillboardHeight(const float height) { billboardHeight_ = height; }
  void SetBaseColor(const Math::Vector4 &color) { baseColor_ = color; }
  void SetCheckerboardEnabled(const bool enabled) { checkerboardEnabled_ = enabled; }
  void SetCheckerColorA(const Math::Vector4 &color) { checkerColorA_ = color; }
  void SetCheckerColorB(const Math::Vector4 &color) { checkerColorB_ = color; }
  void SetDrawCellGridLines(const bool enabled) {
    drawCellGridLines_ = enabled;
  }
  void SetGridLineColor(const Math::Vector4 &color) { gridLineColor_ = color; }
  void SetGridPlaneYOffset(const float offset) { gridPlaneYOffset_ = offset; }

private:
  void RebuildInstanceLayout();
  void DrawCellFloorGrid(AtrumEngine *engine, const Math::Vector3 &origin) const;
  float ResolvedBillboardWidth() const;
  float ResolvedBillboardHeight() const;

  std::string textureFilePath_{"./Assets/Images/white4x4.png"};
  int gridCountX_ = 10;
  int gridCountY_ = 10;
  float cellPitch_ = 1.35f;
  float cellFill_ = 0.78f;
  bool useAutoSizeInCell_ = true;
  float billboardWidth_ = 0.85f;
  float billboardHeight_ = 0.85f;
  Math::Vector4 baseColor_{1.0f, 1.0f, 1.0f, 1.0f};
  bool checkerboardEnabled_ = true;
  Math::Vector4 checkerColorA_{0.92f, 0.96f, 1.05f, 1.0f};
  Math::Vector4 checkerColorB_{0.42f, 0.52f, 0.68f, 1.0f};
  bool drawCellGridLines_ = true;
  Math::Vector4 gridLineColor_{0.35f, 0.4f, 0.48f, 0.95f};
  float gridPlaneYOffset_ = -0.55f;

  uint32_t textureIndex_ = 0;
  std::vector<Math::Vector3> localOffsets_{};
  std::vector<ParticleInstanceGpuData> gpuBatch_{};
};

} // namespace Atrum
