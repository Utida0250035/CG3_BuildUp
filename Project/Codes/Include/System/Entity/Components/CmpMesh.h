#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "Engine/Resource/AssetModel.h"
#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpMesh : public ComponentDraw {
private:
  AssetModel *model_ = nullptr;
  Math::Vector4 color_ = Math::Vector4::White();
  float radius_ = 0.5f;
  bool isLightingEnabled_ = true;

public:
  void Initialize() override;

  void Draw() override;

  // model の寿命は呼び出し側が持つ
  void SetModel(AssetModel *model) { model_ = model; }
  void SetColor(const Math::Vector4 &color) { color_ = color; }
  void SetRadius(const float radius) { radius_ = radius; }

  void SetLightingEnabled(const bool enabled) { isLightingEnabled_ = enabled; }
};

} // namespace Atrum