#include "System/AtomF/CmpAtomFArenaContainment.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFArenaContainment::Update() {
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  AtomF::ClampWorldTranslateToArenaDisc(translate, bodyRadiusMeters_);
}

} // namespace Atrum