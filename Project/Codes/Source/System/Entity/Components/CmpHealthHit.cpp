#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/DeltaTime.h"
#include "Audio/Audio.h"

namespace Atrum {

void CmpHealthHit::ResolveDependence() {
	cmpHitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
	frameDeltaTime_ = FrameDeltaTime::Instance();
}

void CmpHealthHit::Initialize() {
	healthCount_ = initialHealthCount_;
	invincibleTimer_ = 0.0f;
}

void CmpHealthHit::Update() {

	isHurt_ = false;

	if (invincibleTimer_ > 0.0f) {
		invincibleTimer_ -= frameDeltaTime_->GetDeltaTime();
		if (invincibleTimer_ <= 0.0f) {
			invincibleTimer_ = 0.0f;
		}
	} else {

		std::vector<Entity*> hitEntitys = cmpHitSphere_->GetHitEntitys();

		for (const Entity* hitEntity : hitEntitys) {

			if (healthCount_ <= 0) {
				break;
			}

			if (hurterTypes_.contains(hitEntity->GetEntityType())) {
				isHurt_ = true;
				if (healthCount_ > 0) {
					healthCount_--;
					invincibleTimer_ = invincibleTime_;
					Atrum::Audio::Manager::Instance()->Play(Atrum::Audio::Manager::Instance()->Get("./Resources/seHit.mp3"));
				}

				break;
			}
		}
	}
}

} // namespace Atrum