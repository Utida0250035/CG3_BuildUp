#include "System/CmpDrawAttackTelegraph.h"

#include "System/AttackTelegraph/AttackTelegraphDraw.h"
#include "System/CmpAttackTelegraph.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpDrawAttackTelegraph::ResolveDependence() {
  cmpAttackTelegraph_ = RefOwner().GetDataCmp<CmpAttackTelegraph>();
}

void CmpDrawAttackTelegraph::Draw() {
  if (cmpAttackTelegraph_ == nullptr) {
    return;
  }

  DrawAttackTelegraphSegments(cmpAttackTelegraph_->GetSegments());
}

} // namespace Atrum
