#include "Camera/CombatCameraShake.h"
#include "Camera/PlayerDamageScreenVignette.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/DeltaTime.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/PlaySe.h"

namespace Atrum {

namespace {

void PlayPlayerDamageSe() {
	const std::string sePath = PlaySeDetail::ResolveExistingSePath(
	    {"./Assets/Audios/sePlayerDamage.mp3",
	     "./Resources/sePlayerDamage.mp3"});
	if (sePath.empty()) {
		return;
	}

	PlaySe(sePath);
}

EntityTagRegistry &Tags() {
	EntityTagRegistry &registry = EntityTagRegistry::Instance();
	registry.EnsureLoaded();
	return registry;
}

} // namespace

Math::Vector4 CmpHealthHit::GetHurtFlashColor() const {
	if (Tags().EntityHasRole(*GetOwner(), "enemy")) {
		return {1.0f, 0.12f, 0.1f, 0.45f};
	}

	return {1.0f, 0.12f, 0.1f, 1.0f};
}

Math::Vector4 CmpHealthHit::GetHurtFlashInnerColor() const {
	if (Tags().EntityHasRole(*GetOwner(), "enemy")) {
		return {1.0f, 0.12f, 0.1f, 1.0f};
	}

	return GetHurtFlashColor();
}

void CmpHealthHit::ResolveDependence() {
	cmpHitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
	frameDeltaTime_ = FrameDeltaTime::Instance();
}

void CmpHealthHit::Initialize() {
	healthCount_ = initialHealthCount_;
	invincibleTimer_ = 0.0f;
}

bool CmpHealthHit::TryTakeDamageFrom(const Entity *attacker) {
	if (attacker == nullptr || healthCount_ <= 0) {
		return false;
	}

	const EntityTagRegistry &tags = Tags();
	const bool isStackedReflectedBullet =
	    stackReflectedBulletDamage_ && tags.EntityHasRole(*attacker, "playerItem");
	if (!isStackedReflectedBullet && invincibleTimer_ > 0.0f) {
		return false;
	}

	bool isRegisteredHurter = false;
	for (const uint64_t tagHash : attacker->GetTagHashes()) {
		if (hurterTagHashes_.contains(tagHash)) {
			isRegisteredHurter = true;
			break;
		}
	}
	if (!isRegisteredHurter) {
		return false;
	}

	isHurt_ = true;
	healthCount_--;
	hurtFlashTimer_ = kHealthHitHurtFlashSeconds;
	if (!isStackedReflectedBullet) {
		invincibleTimer_ = invincibleTime_;
	}
	if (tags.EntityHasRole(RefOwner(), "player")) {
		PlayPlayerDamageSe();
		CombatCameraShake::Instance().RequestPlayerHitShake();
		PlayerDamageScreenVignette::Instance().RequestShow();
	} else if (tags.EntityHasRole(RefOwner(), "enemy")) {
		PlaySeCandidates({"./Assets/Audios/seHit.mp3", "./Resources/seHit.mp3"});
		CombatCameraShake::Instance().RequestBossHitShake();
	} else {
		PlaySeCandidates({"./Assets/Audios/seHit.mp3", "./Resources/seHit.mp3"});
	}
	if (tags.EntityHasRole(*attacker, "enemyWeapon") &&
	    !attacker->HasUpdCmp<CmpLhRigidBody>()) {
		Particle::Combat::PlayPlayerHitByEnemyBulletEffect(
		    attacker->GetWorldPosition());
	}
	return true;
}

void CmpHealthHit::Update() {

	isHurt_ = false;

	if (hurtFlashTimer_ > 0.0f) {
		hurtFlashTimer_ -= frameDeltaTime_->GetDeltaTime();
		if (hurtFlashTimer_ < 0.0f) {
			hurtFlashTimer_ = 0.0f;
		}
	}

	if (invincibleTimer_ > 0.0f) {
		invincibleTimer_ -= frameDeltaTime_->GetDeltaTime();
		if (invincibleTimer_ <= 0.0f) {
			invincibleTimer_ = 0.0f;
		}
		return;
	}

	const EntityTagRegistry &tags = Tags();
	for (const Entity *hitEntity : cmpHitSphere_->GetHitEntitys()) {
		if (healthCount_ <= 0) {
			break;
		}

		if (stackReflectedBulletDamage_ &&
		    tags.EntityHasRole(*hitEntity, "playerItem")) {
			continue;
		}

		if (TryTakeDamageFrom(hitEntity)) {
			break;
		}
	}
}

} // namespace Atrum
