#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"

#include <memory>
#include <utility>

namespace Atrum {

class AssetModel;

class CmpMeshSphere : public ComponentDraw {
private:
  std::shared_ptr<AssetModel> model_;
  Math::Vector4 colorVec_ = Math::Vector4::White();
  Math::Vector3 drawOffset_{};
  float radius_ = 1.0f;
  uint32_t subdivision_ = 8;
  bool isLightingEnabled_ = true;

public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::MESH_BODY;
  }

  void Draw() override;

  /// 設定すると球の代わりに、半径 1 基準で作ったモデルを半径倍して描く。
  void SetModel(std::shared_ptr<AssetModel> model) { model_ = std::move(model); }
  void SetColor(const Math::Vector4 &color) { colorVec_ = color; }
  void SetDrawOffset(const Math::Vector3 &offset) { drawOffset_ = offset; }
  const Math::Vector3 &GetDrawOffset() const { return drawOffset_; }
  void SetRadius(const float radius) { radius_ = radius; }
  void SetSubdivision(const uint32_t subdivision) {
    subdivision_ = subdivision;
  }

  void SetLightingEnabled(const bool enabled) { isLightingEnabled_ = enabled; }
};

} // namespace Atrum
