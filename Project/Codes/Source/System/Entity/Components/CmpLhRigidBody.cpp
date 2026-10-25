#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/DeltaTime.h"
#include "System/StageBounds.h"

namespace Atrum {

void CmpLhRigidBody::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpLhRigidBody::Initialize() { velocity_ = {}; }

void CmpLhRigidBody::Update() {
	Math::Vector3& translate = RefOwner().RefTransform().translate;

	if (isUseGravity_) {
		velocity_ += gravity_ * frameDeltaTime_->GetDeltaTime();
	}

	translate += velocity_ * frameDeltaTime_->GetDeltaTime();

	if (!isUseSpaceEnd_) {
		return;
	}

	if (translate.y <= kGroundHeight) {
		velocity_.y = 0.0f;
		translate.y = kGroundHeight;
	} else if (translate.y > kCeilHeight) {
		velocity_.y = 0.0f;
		translate.y = kCeilHeight;
	}

	if (translate.x <= kLeftEndX) {
		translate.x = kLeftEndX;
	} else if (translate.x >= kRightEndX) {
		translate.x = kRightEndX;
	}
}

} // namespace Atrum