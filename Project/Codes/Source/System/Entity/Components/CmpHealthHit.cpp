#include "Camera/CombatCameraShake.h"
#include "Camera/PlayerDamageScreenVignette.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
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

} // namespace

Math::Vector4 CmpHealthHit::GetHurtFlashColor() const {
	if (GetOwner()->GetEntityType() == EntityType::ENEMY) {
		return {1.0f, 0.12f, 0.1f, 0.45f};
	}

	return {1.0f, 0.12f, 0.1f, 1.0f};
}

Math::Vector4 CmpHealthHit::GetHurtFlashInnerColor() const {
	if (GetOwner()->GetEntityType() == EntityType::ENEMY) {
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

	const bool isStackedReflectedBullet =
	    stackReflectedBulletDamage_ &&
	    attacker->GetEntityType() == EntityType::PLAYER_ITEM;
	if (!isStackedReflectedBullet && invincibleTimer_ > 0.0f) {
		return false;
	}

	if (!hurterTypes_.contains(attacker->GetEntityType())) {
		return false;
	}

	isHurt_ = true;
	healthCount_--;
	hurtFlashTimer_ = kHealthHitHurtFlashSeconds;
	if (!isStackedReflectedBullet) {
		invincibleTimer_ = invincibleTime_;
	}
	if (RefOwner().GetEntityType() == EntityType::PLAYER) {
		PlayPlayerDamageSe();
		CombatCameraShake::Instance().RequestPlayerHitShake();
		PlayerDamageScreenVignette::Instance().RequestShow();
	} else if (RefOwner().GetEntityType() == EntityType::ENEMY) {
		PlaySeCandidates({"./Assets/Audios/seHit.mp3", "./Resources/seHit.mp3"});
		CombatCameraShake::Instance().RequestBossHitShake();
	} else {
		PlaySeCandidates({"./Assets/Audios/seHit.mp3", "./Resources/seHit.mp3"});
	}
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

	for (const Entity *hitEntity : cmpHitSphere_->GetHitEntitys()) {
		if (healthCount_ <= 0) {
			break;
		}

		// 反射弾は CmpReflectAbleBullet::LateResolveCombatHits で1発1ダメージに確定する
		if (stackReflectedBulletDamage_ &&
		    hitEntity->GetEntityType() == EntityType::PLAYER_ITEM) {
			continue;
		}

		if (TryTakeDamageFrom(hitEntity)) {
			break;
		}
	}
}

} // namespace Atrum