#include "System/AtomF/AtomFBossShockwave.h"

#include "Math/Transform.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/CmpAtomFBossShockwave.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "System/Entity/Entity.h"

namespace Atrum::AtomF {

void SpawnBossShockwave(const Math::Vector3 &centerWorld,
                        const float maxRadiusMeters,
                        const float speedMetersPerSecond,
                        const float bandHalfWidthMeters) {
  std::unique_ptr<Entity> wave = std::make_unique<Entity>("AtomFBossShockwave");

  Math::TransformLH initialTransform{};
  initialTransform.translate = {centerWorld.x, 0.0f, centerWorld.z};
  wave->SetInitialTransform(initialTransform);

  wave->AddUpdCmp<CmpAtomFBossShockwave>();
  if (CmpAtomFBossShockwave *shockwave =
          wave->GetUpdCmp<CmpAtomFBossShockwave>()) {
    shockwave->Setup(initialTransform.translate, maxRadiusMeters,
                     speedMetersPerSecond, bandHalfWidthMeters);
  }
  wave->AddDrawCmp<CmpDrawAtomFTelegraph>();

  wave->ResolveDependencies();
  wave->Initialize();
  EnqueueSceneEntityAdopt(std::move(wave));
}

} // namespace Atrum::AtomF
