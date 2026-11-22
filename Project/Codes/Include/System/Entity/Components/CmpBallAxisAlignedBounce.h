#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 軸平行な壁・床での半径付き球バウンス（運動積分・演出とは分離）
class CmpBallAxisAlignedBounce : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PHYSICS;
  }

  void Update() override;

  void SetLeftWallX(const float x) { leftWallX_ = x; }
  void SetRightWallX(const float x) { rightWallX_ = x; }
  void SetGroundY(const float y) { groundY_ = y; }
  void SetRadius(const float radius) { radius_ = radius; }
  void SetWallRestitution(const float value) { wallRestitution_ = value; }
  void SetFloorRestitution(const float value) { floorRestitution_ = value; }

private:
  float leftWallX_ = -5.0f;
  float rightWallX_ = 5.0f;
  float groundY_ = -3.68f;
  float radius_ = 0.32f;
  float wallRestitution_ = 0.94f;
  float floorRestitution_ = 0.88f;
};

} // namespace Atrum