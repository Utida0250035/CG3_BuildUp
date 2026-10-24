#include "System/CmpDrawBullet.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpDrawBullet::ResolveDependence() {
  cmpBullet_ = RefOwner().GetUpdCmp<CmpReflectAbleBullet>();
}

void CmpDrawBullet::Draw() {
  for (auto &bullet : cmpBullet_->RefBullets()) {
    if (bullet) {
      bullet->Draw();
    }
  }
}

} // namespace Atrum
