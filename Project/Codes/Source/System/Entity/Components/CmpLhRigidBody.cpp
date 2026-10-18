#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/DeltaTime.h"

namespace Atrum {

void CmpLhRigidBody::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpLhRigidBody::Initialize() { velocity_ = {}; }

void CmpLhRigidBody::Update() {
	Math::Vector3& translate = RefOwner().RefTransform().translate;

	if (isUseGravity_) {
		velocity_ += gravity_ * frameDeltaTime_->GetDeltaTime();
	}

	translate += velocity_ * frameDeltaTime_->GetDeltaTime();
}

} // namespace Atrum