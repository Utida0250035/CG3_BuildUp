#pragma once

#include "System/Entity/ComponentData.h"
#include <string>

namespace Atrum {

/// エディタ用: 外部アセットへの相対パス（JSON・シーン保存向け）
class CmpEditorAssetPath : public ComponentData {
private:
  std::string assetPath_{};
  bool loadObjAsMesh_ = true;
  bool loadImageAsBillboard_ = true;
  float billboardWidth_ = 1.0f;
  float billboardHeight_ = 1.0f;

public:
  const std::string &AssetPath() const { return assetPath_; }
  void SetAssetPath(const std::string &path) { assetPath_ = path; }

  bool LoadObjAsMesh() const { return loadObjAsMesh_; }
  void SetLoadObjAsMesh(const bool enabled) { loadObjAsMesh_ = enabled; }

  bool LoadImageAsBillboard() const { return loadImageAsBillboard_; }
  void SetLoadImageAsBillboard(const bool enabled) {
    loadImageAsBillboard_ = enabled;
  }

  float BillboardWidth() const { return billboardWidth_; }
  float BillboardHeight() const { return billboardHeight_; }
  void SetBillboardSize(const float width, const float height) {
    billboardWidth_ = width;
    billboardHeight_ = height;
  }
};

} // namespace Atrum
