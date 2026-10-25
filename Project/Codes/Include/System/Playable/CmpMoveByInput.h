#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/BehaviorBox.h"
#include "System/Entity/ComponentUpd.h"
#include "System/Playable/CmpInput.h"

namespace Atrum {

class CmpDirection;
class CmpHealthHit;
class CmpHitSphere;
class Entity;
class FrameDeltaTime;
class CmpSwing;
class CmpMesh;

namespace Player {

class CmpDrawPlayerParryField;

class CmpMoveByInput2D : public ComponentUpd {
private:
  CmpInput *cmpInput_ = nullptr;
  InputData inputData_{};
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  BehaviorBox behaviorBox_{};

  CmpHitSphere *parryCmpHitSphere_ = nullptr;
  CmpHitSphere *cmpHitSphere_ = nullptr;
  CmpSwing *cmpSwingWeapon_ = nullptr;
  CmpMesh *cmpMesh_ = nullptr;
  CmpHealthHit *cmpHealthHit_ = nullptr;
  CmpDrawPlayerParryField *cmpDrawPlayerParryField_ = nullptr;
  CmpDirection *cmpDirection_ = nullptr;

  Entity *directionMarkerForward_ = nullptr;
  Entity *directionMarkerRear_ = nullptr;
  float directionMarkerFaceSignX_ = 1.0f;
  float directionMarkerSideOffset_ = 0.65f;

  Math::Vector4 invincibleColor = {0.75f, 0.75f, 0.1f, 1.0f};
  bool isInvincibleColorWaitNextFrame_ = false;

  float parryDuration_ = 0.25f;
  float parryTimer_ = 0.0f;
  float invincibleDuration_ = 0.03125f;
  float invincibleTimer_ = 0.0f;
  bool isAirJumpVariable_ = false;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::MOVEMENT; }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;

  BehaviorBox &RefBehaviorBox() { return behaviorBox_; }

  void SetDirectionMarkers(Entity *forwardMarker, Entity *rearMarker,
                           const float sideOffset);
  void SetInitialDirectionFaceSignX(const float signX) {
    directionMarkerFaceSignX_ = signX;
  }

private:
  void UpdateDirectionMarkerPositions();
};

} // namespace Player

} // namespace Atrum
