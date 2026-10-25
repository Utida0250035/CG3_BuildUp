#include "System/CmpDrawBullet.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpDrawBullet::ResolveDependence() {
  if (RefOwner().HasUpdCmp<CmpReflectAbleBullet>()) {
    cmpBullet_ = RefOwner().GetUpdCmp<CmpReflectAbleBullet>();
  } else {
    cmpBullet_ = nullptr;
  }
}

void CmpDrawBullet::Draw() {
  if (cmpBullet_ == nullptr) {
    return;
  }

  for (auto &bullet : cmpBullet_->RefBullets()) {
    if (bullet) {
      bullet->Draw();
    }
  }
}

} // namespace Atrum
