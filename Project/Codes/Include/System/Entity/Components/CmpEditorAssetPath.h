#pragma once

#include "System/Entity/ComponentData.h"
#include <string>

namespace Atrum {

/// エディタ用: 外部アセットへの相対パス（JSON・シーン保存向け）
class CmpEditorAssetPath : public ComponentData {
private:
  std::string assetPath_{};
  bool loadObjAsMesh_ = true;

public:
  const std::string &AssetPath() const { return assetPath_; }
  void SetAssetPath(const std::string &path) { assetPath_ = path; }

  bool LoadObjAsMesh() const { return loadObjAsMesh_; }
  void SetLoadObjAsMesh(const bool enabled) { loadObjAsMesh_ = enabled; }
};

} // namespace Atrum
