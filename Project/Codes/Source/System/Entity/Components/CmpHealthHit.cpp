#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/DeltaTime.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/PlaySe.h"

namespace Atrum {

void CmpHealthHit::ResolveDependence() {
	cmpHitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
	frameDeltaTime_ = FrameDeltaTime::Instance();
}

void CmpHealthHit::Initialize() {
	healthCount_ = initialHealthCount_;
	invincibleTimer_ = 0.0f;
}

bool CmpHealthHit::TryTakeDamageFrom(const Entity *attacker) {
	if (attacker == nullptr || healthCount_ <= 0 || invincibleTimer_ > 0.0f) {
		return false;
	}

	if (!hurterTypes_.contains(attacker->GetEntityType())) {
		return false;
	}

	isHurt_ = true;
	healthCount_--;
	invincibleTimer_ = invincibleTime_;
	PlaySe("./Resources/seHit.mp3");
	// 管理外の ENEMY_WEAPON（オービター等）被弾用。管理弾は CmpReflectAbleBullet 側で演出
	if (attacker->GetEntityType() == EntityType::ENEMY_WEAPON &&
	    !attacker->HasUpdCmp<CmpLhRigidBody>()) {
		Particle::Combat::PlayPlayerHitByEnemyBulletEffect(
		    attacker->GetWorldPosition());
	}
	return true;
}

void CmpHealthHit::Update() {

	isHurt_ = false;

	if (invincibleTimer_ > 0.0f) {
		invincibleTimer_ -= frameDeltaTime_->GetDeltaTime();
		if (invincibleTimer_ <= 0.0f) {
			invincibleTimer_ = 0.0f;
		}
		return;
	}

	for (const Entity *hitEntity : cmpHitSphere_->GetHitEntitys()) {
		if (healthCount_ <= 0) {
			break;
		}

		if (TryTakeDamageFrom(hitEntity)) {
			break;
		}
	}
}

} // namespace Atrum