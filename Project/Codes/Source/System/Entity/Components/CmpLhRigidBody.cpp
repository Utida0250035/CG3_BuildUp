#include "System/Entity/Components/CmpLhRigidBody.h"

#include "Engine/EngineAccess.h"
#include "Physics/PhysicsManager.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpLhRigidBody::ResolveDependence() { frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime(); }

void CmpLhRigidBody::Initialize() { velocity_ = {}; }

void CmpLhRigidBody::Update() {
	if (frameDeltaTime_ == nullptr) {
		return;
	}

	Physics::PhysicsManager *physics = ::Atrum::EnginePhysicsManager();
	if (physics == nullptr) {
		return;
	}

	Physics::KinematicStepSettings settings{};
	settings.gravity = gravity_;
	settings.useGravity = isUseGravity_;
	settings.clampToStageBounds = isUseSpaceEnd_;

	physics->StepKinematic(RefOwner().RefTransform().translate, velocity_, settings,
	                       frameDeltaTime_->GetDeltaTime());
}

} // namespace Atrum