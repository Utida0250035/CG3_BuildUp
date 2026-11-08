#pragma once

#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"

namespace Atrum {

/// エディタプレビュー用: インポート画像のビルボード表示
class CmpEditorTextureBillboard : public ComponentDraw {
private:
  uint32_t textureIndex_ = 0;
  float width_ = 1.0f;
  float height_ = 1.0f;
  Math::Vector4 color_ = Math::Vector4::White();

public:
  void Draw() override;

  void SetTextureIndex(const uint32_t index) { textureIndex_ = index; }
  void SetSize(const float width, const float height) {
    width_ = width;
    height_ = height;
  }
};

} // namespace Atrum
