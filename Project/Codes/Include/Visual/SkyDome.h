#pragma once

#include "Math/Vector4.h"

namespace Atrum {

class AssetModel;
class Camera;

/// <summary>
/// 天球（カメラ追従・最遠景描画）。SkyDome.obj は内側から見えるよう面が反転されている。
/// </summary>
class SkyDome {
public:
  void Initialize(Camera *camera, AssetModel *model = nullptr);

  void Update();

  void Draw();

  void SetCamera(Camera *camera) { camera_ = camera; }

  void SetColor(const Math::Vector4 &color) { color_ = color; }
  void SetRadius(const float radius) { radius_ = radius; }
  void SetSubdivision(const uint32_t subdivision) {
    subdivision_ = subdivision;
  }

private:
  Camera *camera_ = nullptr;
  AssetModel *model_ = nullptr;
  Math::Vector4 color_{0.12f, 0.08f, 0.22f, 1.0f};
  float radius_ = 96.0f;
  uint32_t subdivision_ = 2;
};

} // namespace Atrum
