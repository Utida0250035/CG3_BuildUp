#include "System/Playable/CmpMoveByInput.h"
#include "System/BehDodge.h"
#include "System/BehWalkAndJump.h"
#include "System/CmpParry.h"
#include "System/CmpSwing.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMesh.h"
#include "System/Entity/Entity.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"

namespace Atrum::Player {

void CmpMoveByInput2D::ResolveDependence() {
  cmpInput_ = CmpInput::Instance();
  cmpInput_->ResolveDependence();
  frameDeltaTime_ = FrameDeltaTime::Instance();

  cmpDirection_ = RefOwner().GetDataCmp<CmpDirection>();
  if (CmpParry *cmpParry = RefOwner().GetDataCmp<CmpParry>();
      cmpParry != nullptr && cmpParry->HasEntParry()) {
    parryCmpHitSphere_ = cmpParry->RefEntParry().GetUpdCmp<CmpHitSphere>();
  }
  cmpHitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
  cmpSwingWeapon_ = RefOwner().GetUpdCmp<CmpSwing>();
  cmpMesh_ = RefOwner().GetDrawCmp<CmpMesh>();
}

void CmpMoveByInput2D::SetDirectionMarkers(Entity *forwardMarker,
                                           Entity *rearMarker,
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
      directionMarkerFaceSignX_ * directionMarkerSideOffset_, 0.0f, 0.0f};
  directionMarkerRear_->RefTransform().translate = {
      -directionMarkerFaceSignX_ * directionMarkerSideOffset_, 0.0f, 0.0f};
}

void CmpMoveByInput2D::Initialize() {
  behaviorBox_.SetGrandOwner(&RefOwner());
  behaviorBox_.Transition(new BehWalkAndJump());
  if (parryCmpHitSphere_ != nullptr) {
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
  cmpHitSphere_->SetIsActive(true);
  cmpMesh_->SetColor(Math::Vector4::White());

  if (inputData_.isJumpTrigger) {
    if (RefOwner().GetWorldPosition().y <= kGroundHeight) {
      parryTimer_ = parryDuration_;
      invincibleTimer_ = invincibleDuration_;
      isAirJumpVariable_ = true;
      isInvincibleColorWaitNextFrame_ = true;
      cmpSwingWeapon_->StartSwing();
      PlaySe("./Resources/seSwingWeapon.mp3");
    } else if (isAirJumpVariable_) {
      PlaySe("./Resources/seSwingWeapon.mp3");
      cmpSwingWeapon_->StartSwing();
      invincibleTimer_ = invincibleDuration_;
      parryTimer_ = parryDuration_;
      isInvincibleColorWaitNextFrame_ = true;
      isAirJumpVariable_ = false;
    }
  }

  if (parryTimer_ > 0.0f && parryCmpHitSphere_ != nullptr) {
    parryCmpHitSphere_->SetIsActive(true);
    parryTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (parryTimer_ < 0.0f) {
      parryTimer_ = 0.0f;
    }
  }

  if (invincibleTimer_ > 0.0f) {
    cmpHitSphere_->SetIsActive(false);
    if (isInvincibleColorWaitNextFrame_) {
      isInvincibleColorWaitNextFrame_ = false;
    } else {
      cmpMesh_->SetColor(invincibleColor);
    }

    invincibleTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (invincibleTimer_ <= 0.0f) {
      invincibleTimer_ = 0.0f;
      cmpMesh_->SetColor(Math::Vector4::White());
    }
  }

  if (inputData_.isDodgeTrigger && dodgeCoolTimer_ <= 0.0f) {
    BehDodge *behDodge = new BehDodge();
    behDodge->SetPBehaviorBox(&behaviorBox_);
    behDodge->SetNextBehavior(new BehWalkAndJump());
    behaviorBox_.Transition(behDodge);
    dodgeCoolTimer_ = dodgeCoolTime_;
    PlaySe("./Resources/seStep.mp3");
  }

  if (dodgeCoolTimer_ > 0.0f) {
    dodgeCoolTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (dodgeCoolTimer_ < 0.0f) {
      dodgeCoolTimer_ = 0.0f;
    }
  }

  UpdateDirectionMarkerPositions();
}

} // namespace Atrum::Player
