#include "../System/CmpLhRigidBody.h"

#include "../ConstantVal.h"
#include "../System/Entity.h"
#include "../Time/DeltaTime.h"

namespace Atrum {

void CmpLhRigidBody::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::GetInstance(); }

void CmpLhRigidBody::Initialize() { velocity_ = {}; }

void CmpLhRigidBody::Update() {
	Math::Vector3& translate = RefOwner().RefTransform().translate;

	if (isUseGravity_) {
		velocity_ += gravity_ * frameDeltaTime_->GetDeltaTime();
	}

	translate += velocity_ * frameDeltaTime_->GetDeltaTime();

	if (isUseSpaceEnd_) {
		if (translate.y <= kGroundHeight) {
			velocity_.y = 0.0f;
			translate.y = -4.0f;
		} else if (translate.y > 8.0f) {
			velocity_.y = 0.0f;
			translate.y = 8.0f;
		}

		if (translate.x <= -14.0f) {
			translate.x = -14.0f;
		} else if (translate.x >= 14.0f) {
			translate.x = 14.0f;
		}
	}
}

} // namespace Atrum