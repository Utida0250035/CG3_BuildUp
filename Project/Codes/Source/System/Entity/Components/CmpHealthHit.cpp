#include "../System/CmpHealthHit.h"
#include "../System/CmpHitSphere.h"
#include "../System/Entity.h"
#include "../Time/DeltaTime.h"
#include "../Audio/Audio.h"

namespace Atrum {

void CmpHealthHit::ResolveDependence() {
	cmpHitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
	frameDeltaTime_ = FrameDeltaTime::GetInstance();
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
					Atrum::Audio::Manager::GetInstance()->Play(Atrum::Audio::Manager::GetInstance()->Get("./Resources/seHit.mp3"));
				}

				break;
			}
		}
	}
}

} // namespace Atrum