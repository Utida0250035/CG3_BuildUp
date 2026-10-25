#include "Visual/SkyDome.h"

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/AssetModel.h"
#include "Math/Transform.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void SkyDome::Initialize(Camera *camera, AssetModel *model) {
  camera_ = camera;
  model_ = model;
}

void SkyDome::Update() {}

void SkyDome::Draw() {
  if (camera_ == nullptr) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();

  // 深度書き込みなし（NORMAL PSO）。メッシュは OBJ 側で内側向きに反転済み
  engine->SetBlendModeForFrame(BlendMode::NORMAL);

  Math::Transform worldTransform{};
  worldTransform.translate = camera_->GetTranslate();
  worldTransform.scale = {radius_, radius_, radius_};
  const Math::Matrix4x4 worldMatrix = worldTransform.MakeWorldMatrix();

  if (model_ != nullptr) {
    engine->DrawModel(model_, worldMatrix, false, Math::Vector4::White());
  } else {
    engine->DrawSphere(WhiteTextureIndex(), color_, Math::Transform{}, worldMatrix,
                       1.0f, subdivision_, false);
  }

  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum
