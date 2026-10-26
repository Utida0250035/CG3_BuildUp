#include "System/AttackTelegraph/AttackTelegraphDraw.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "System/AttackTelegraph/AttackTelegraphSegment.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void DrawAttackTelegraphSegments(
    const std::vector<AttackTelegraphSegment> &segments) {
  if (segments.empty()) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  for (const AttackTelegraphSegment &segment : segments) {
    engine->DrawDashedLine3D(
        WhiteTextureIndex(), segment.color, Math::Transform{}, segment.start,
        segment.end, kAttackTelegraphLineWidth, kAttackTelegraphDashLength,
        kAttackTelegraphDashGap, false);
  }
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum
