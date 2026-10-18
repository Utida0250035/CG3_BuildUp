#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "Engine/Resource/AssetModel.h"
#include "System/Entity/ComponentDraw.h"

namespace Atrum {

class CmpMesh : public ComponentDraw {
private:
  AssetModel *model_ = nullptr;
  Math::Vector4 color_ = Math::Vector4::White();

public:
  void Initialize() override;

  void Draw() override;

  ~CmpMesh() override {
    delete model_;
    model_ = nullptr;
  }

  void SetModel(AssetModel *model) { model_ = model; }
  void SetColor(const Math::Vector4 &color) { color_ = color; }
};

} // namespace Atrum