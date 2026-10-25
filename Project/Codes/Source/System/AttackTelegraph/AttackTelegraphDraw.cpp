#include "System/AttackTelegraph/AttackTelegraphDraw.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "System/AttackTelegraph/AttackTelegraphSegment.h"
#include "System/WhiteTexture.h"

#include <algorithm>

namespace Atrum {

namespace {

void DrawTelegraphSegment(AtrumEngine *engine,
                          const AttackTelegraphSegment &segment) {
  const Math::Vector3 delta = segment.end - segment.start;
  const float length = delta.Length();
  if (length < 1.0e-3f) {
    return;
  }

  const int stepCount = std::max(
      2, static_cast<int>(length / kAttackTelegraphLineSphereStep) + 1);
  const Math::Vector3 step = delta / static_cast<float>(stepCount - 1);

  for (int i = 0; i < stepCount; ++i) {
    Math::Transform transform{};
    transform.translate = segment.start + step * static_cast<float>(i);
    engine->DrawSphere(
        WhiteTextureIndex(), segment.color, Math::Transform{},
        transform.MakeWorldMatrix(), kAttackTelegraphLineSphereRadius,
        kAttackTelegraphLineSphereSubdivision, false);
  }
}

} // namespace

void DrawAttackTelegraphSegments(
    const std::vector<AttackTelegraphSegment> &segments) {
  if (segments.empty()) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  for (const AttackTelegraphSegment &segment : segments) {
    DrawTelegraphSegment(engine, segment);
  }
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum
