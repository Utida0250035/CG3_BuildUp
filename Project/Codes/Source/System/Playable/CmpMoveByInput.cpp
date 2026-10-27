#include "System/Playable/CmpMoveByInput.h"
#include "System/Playable/CmpDrawPlayerParryField.h"
#include "System/BehWalkAndJump.h"
#include "System/Entity/Components/CmpParry.h"
#include "System/Entity/Components/CmpSwing.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpMesh.h"
#include "System/Entity/Entity.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"

namespace Atrum::Player {

	namespace {

		void ApplyDirectionMarkerColor(Entity* marker, const Math::Vector4& baseColor,
			const bool visible) {
			if (marker == nullptr) {
				return;
			}

			CmpMeshSphere* mesh = marker->GetDrawCmp<CmpMeshSphere>();
			if (mesh == nullptr) {
				return;
			}

			Math::Vector4 color = baseColor;
			if (!visible) {
				color.w = 0.0f;
			}
			mesh->SetColor(color);
		}

	} // namespace

	void CmpMoveByInput2D::SetDirectionMarkerBaseColors(
		const Math::Vector4& forwardColor, const Math::Vector4& rearColor) {
		directionMarkerForwardBaseColor_ = forwardColor;
		directionMarkerRearBaseColor_ = rearColor;
	}

	void CmpMoveByInput2D::ResolveDependence() {
		cmpInput_ = CmpInput::Instance();
		cmpInput_->ResolveDependence();
		frameDeltaTime_ = FrameDeltaTime::Instance();

		cmpDirection_ = RefOwner().GetDataCmp<CmpDirection>();
		if (CmpParry* cmpParry = RefOwner().GetDataCmp<CmpParry>();
			cmpParry != nullptr && cmpParry->HasEntParry()) {
			parryCmpHitSphere_ = cmpParry->RefEntParry().GetUpdCmp<CmpHitSphere>();
		}
		cmpHitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
		cmpSwingWeapon_ = RefOwner().GetUpdCmp<CmpSwing>();
		cmpMesh_ = RefOwner().GetDrawCmp<CmpMesh>();
		cmpHealthHit_ = RefOwner().GetUpdCmp<CmpHealthHit>();
		if (RefOwner().HasDrawCmp<CmpDrawPlayerParryField>()) {
			cmpDrawPlayerParryField_ =
				RefOwner().GetDrawCmp<CmpDrawPlayerParryField>();
		}
	}

	void CmpMoveByInput2D::SetDirectionMarkers(Entity* forwardMarker,
		Entity* rearMarker,
		const float sideOffset) {
		directionMarkerForward_ = forwardMarker;
		directionMarkerRear_ = rearMarker;
		directionMarkerSideOffset_ = sideOffset;
		UpdateDirectionMarkerPositions();
	}

	void CmpMoveByInput2D::UpdateDirectionMarkerPositions() {
		if (directionMarkerForward_ == nullptr || directionMarkerRear_ == nullptr ||
			cmpDirection_ == nullptr) {
			return;
		}

		const float directionX = cmpDirection_->GetDirection().x;
		if (directionX != 0.0f) {
			directionMarkerFaceSignX_ = directionX > 0.0f ? 1.0f : -1.0f;
		}

		directionMarkerForward_->RefTransform().translate = {
			directionMarkerFaceSignX_ * directionMarkerSideOffset_, 0.0f, 0.0f };
		directionMarkerRear_->RefTransform().translate = {
			-directionMarkerFaceSignX_ * directionMarkerSideOffset_, 0.0f, 0.0f };
	}

	void CmpMoveByInput2D::Initialize() {
		behaviorBox_.SetGrandOwner(&RefOwner());
		behaviorBox_.Transition(std::make_unique<BehWalkAndJump>().release());
		if (parryCmpHitSphere_) {
			parryCmpHitSphere_->SetIsActive(false);
		}
		parryTimer_ = 0.0f;
		invincibleTimer_ = 0.0f;
		UpdateDirectionMarkerPositions();
	}

	void CmpMoveByInput2D::Update() {
		cmpInput_->Update();
		inputData_ = cmpInput_->GetData();
		behaviorBox_.Execute();

		if (parryCmpHitSphere_ != nullptr) {
			parryCmpHitSphere_->SetIsActive(false);
		}
		if (cmpDrawPlayerParryField_ != nullptr) {
			cmpDrawPlayerParryField_->SetVisible(false);
		}
		cmpHitSphere_->SetIsActive(true);

		if (inputData_.isJumpTrigger) {
			if (RefOwner().GetWorldPosition().y <= kGroundHeight) {
				parryTimer_ = parryDuration_;
				invincibleTimer_ = invincibleDuration_;
				isAirJumpVariable_ = true;
				isInvincibleColorWaitNextFrame_ = true;
				cmpSwingWeapon_->StartSwing();
				PlaySeCandidates({ "./Assets/Audios/seSwingWeapon.mp3",
								  "./Resources/seSwingWeapon.mp3" });
			} else if (isAirJumpVariable_) {
				PlaySeCandidates({ "./Assets/Audios/seSwingWeapon.mp3",
								  "./Resources/seSwingWeapon.mp3" });
				cmpSwingWeapon_->StartSwing();
				invincibleTimer_ = invincibleDuration_;
				parryTimer_ = parryDuration_;
				isInvincibleColorWaitNextFrame_ = true;
				isAirJumpVariable_ = false;
			}
		}

		if (parryTimer_ > 0.0f && parryCmpHitSphere_ != nullptr) {
			parryCmpHitSphere_->SetIsActive(true);
			if (cmpDrawPlayerParryField_ != nullptr) {
				cmpDrawPlayerParryField_->SetVisible(true);
			}
			parryTimer_ -= frameDeltaTime_->GetDeltaTime();
			if (parryTimer_ < 0.0f) {
				parryTimer_ = 0.0f;
			}
		}

		const bool damageInvincible =
			cmpHealthHit_ != nullptr && cmpHealthHit_->IsInvincible();

		if (damageInvincible) {
			if (!wasDamageInvincible_) {
				damageInvincibleBlinkTimer_ = 0.0f;
			}

			constexpr float kDamageInvincibleBlinkPeriod = 0.12f;
			damageInvincibleBlinkTimer_ += frameDeltaTime_->GetDeltaTime();
			const bool showMesh =
				static_cast<int>(damageInvincibleBlinkTimer_ /
					kDamageInvincibleBlinkPeriod) % 2 == 0;

			cmpHitSphere_->SetIsActive(false);

			if (showMesh) {
				cmpMesh_->SetColor(kDefaultColor);
			} else {
				cmpMesh_->SetColor(Math::Vector4::Zero());
			}

			ApplyDirectionMarkerColor(directionMarkerForward_,
				directionMarkerForwardBaseColor_, showMesh);
			ApplyDirectionMarkerColor(directionMarkerRear_,
				directionMarkerRearBaseColor_, showMesh);
		} else {
			damageInvincibleBlinkTimer_ = 0.0f;

			if (invincibleTimer_ > 0.0f) {
				cmpHitSphere_->SetIsActive(false);
				if (isInvincibleColorWaitNextFrame_) {
					isInvincibleColorWaitNextFrame_ = false;
					cmpMesh_->SetColor(kDefaultColor);
				} else {
					cmpMesh_->SetColor(invincibleColor);
				}

				invincibleTimer_ -= frameDeltaTime_->GetDeltaTime();
				if (invincibleTimer_ <= 0.0f) {
					invincibleTimer_ = 0.0f;
				}
			} else {
				cmpMesh_->SetColor(kDefaultColor);
				ApplyDirectionMarkerColor(directionMarkerForward_,
					directionMarkerForwardBaseColor_, true);
				ApplyDirectionMarkerColor(directionMarkerRear_,
					directionMarkerRearBaseColor_, true);
			}
		}

		wasDamageInvincible_ = damageInvincible;

		UpdateDirectionMarkerPositions();
	}

} // namespace Atrum::Player
