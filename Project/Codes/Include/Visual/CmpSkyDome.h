#pragma once

#include "System/Entity/ComponentDraw.h"
#include "Visual/SkyDome.h"

#include <memory>

namespace Atrum {

class AssetModel;
class Camera;

/// <summary>
/// 天球描画コンポーネント（Entity 経由でシーンに載せる）
/// </summary>
class CmpSkyDome : public ComponentDraw {
public:
  void Initialize() override;

  void Draw() override;

  void SetCamera(Camera *camera);
  void SetModel(const std::shared_ptr<AssetModel> &model);
  SkyDome &RefSkyDome() { return skyDome_; }

private:
  Camera *camera_ = nullptr;
  std::shared_ptr<AssetModel> model_;
  SkyDome skyDome_{};
};

} // namespace Atrum
