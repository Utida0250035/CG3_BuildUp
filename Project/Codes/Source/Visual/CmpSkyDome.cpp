#include "Visual/CmpSkyDome.h"

#include "Camera/Camera.h"
namespace Atrum {

void CmpSkyDome::Initialize() {
  skyDome_.Initialize(camera_, model_.get());
  // カメラ far（128）より手前に収まるよう、単位球メッシュ想定で拡大
  skyDome_.SetRadius(100.0f);
}

void CmpSkyDome::Draw() { skyDome_.Draw(); }

void CmpSkyDome::SetCamera(Camera *camera) {
  camera_ = camera;
  skyDome_.SetCamera(camera);
}

void CmpSkyDome::SetModel(const std::shared_ptr<AssetModel> &model) {
  model_ = model;
}

} // namespace Atrum
