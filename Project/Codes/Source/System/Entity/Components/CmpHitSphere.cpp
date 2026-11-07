#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/HitJudge.h"

namespace Atrum {

void CmpHitSphere::ResolveDependence() {
  hitJudge_ = HitJudge::Instance();
  if (!isRegisteredWithHitJudge_) {
    hitJudge_->Register(this);
    isRegisteredWithHitJudge_ = true;
  }
}

void CmpHitSphere::Initialize() { hitEntitys_ = {}; }

void CmpHitSphere::Update() { hitEntitys_ = hitJudge_->IsHitOthers(this); }

std::vector<Entity*> CmpHitSphere::GetHitEntitys() { return hitEntitys_; }

void CmpHitSphere::Finalize() {
  if (!isRegisteredWithHitJudge_ || hitJudge_ == nullptr) {
    return;
  }
  hitJudge_->Remove(this);
  isRegisteredWithHitJudge_ = false;
}

} // namespace Atrum