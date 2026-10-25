#include "System/Playable/CmpDrawPlayerParryField.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum::Player {

void CmpDrawPlayerParryField::Draw() {
  if (!isVisible_) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  if (color_.w < 0.999f) {
    engine->SetBlendModeForFrame(BlendMode::NORMAL);
  }

  engine->DrawSphere(WhiteTextureIndex(), color_, Math::Transform{},
                     GetOwner()->GetWorldMatrix(), radius_, 14, false);

  if (color_.w < 0.999f) {
    engine->SetBlendModeForFrame(BlendMode::NONE);
  }
}

} // namespace Atrum::Player
