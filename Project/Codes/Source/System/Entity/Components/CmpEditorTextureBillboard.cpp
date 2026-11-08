#include "System/Entity/Components/CmpEditorTextureBillboard.h"

#include "Engine/AtrumEngine.h"
#include "Math/Transform.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpEditorTextureBillboard::Draw() {
  if (textureIndex_ == 0) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine == nullptr) {
    return;
  }

  Math::Transform uvTransform{};
  uvTransform.scale = Math::Vector3{width_, height_, 1.0f};
  engine->DrawBillboard(textureIndex_, color_, uvTransform,
                        RefOwner().GetWorldMatrix(), false);
}

} // namespace Atrum
