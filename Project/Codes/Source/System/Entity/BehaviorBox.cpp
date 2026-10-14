#include "../System/BehaviorBox.h"

#include "../System/Behavior.h"
#include "../System/Entity.h"

namespace Atrum {

void BehaviorBox::Transition(Behavior* newBehavior) {
	if (behavior_) {
		behavior_->Finalize();
	}

	behavior_.reset(newBehavior);

	assert(grandOwner_);

	behavior_->SetGrandOwner(grandOwner_);
	behavior_->ResolveDependence();

	behavior_->Initialize();

	transitionCount_++;
}

void BehaviorBox::Execute() {
	if (!behavior_) {
		return;
	}

	behavior_->Execute();
}

void BehaviorBox::Finalize() {
	if (!behavior_) {
		return;
	}

	behavior_->Finalize();
}

void BehaviorBox::SetGrandOwner(Entity* entity) { grandOwner_ = entity; }

} // namespace Atrum