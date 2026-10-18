#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/HitJudge.h"

namespace Atrum {

void CmpHitSphere::ResolveDependence() {
	hitJudge_ = HitJudge::GetInstance();
	hitJudge_->Register(this);
}

void CmpHitSphere::Initialize() { hitEntitys_ = {}; }

void CmpHitSphere::Update() { hitEntitys_ = hitJudge_->IsHitOthers(this); }

std::vector<Entity*> CmpHitSphere::GetHitEntitys() { return hitEntitys_; }

void CmpHitSphere::Finalize() {

	hitJudge_->Remove(this);
};

} // namespace Atrum