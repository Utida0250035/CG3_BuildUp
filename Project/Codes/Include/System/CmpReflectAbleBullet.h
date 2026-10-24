#pragma once

#include "System/Entity/ComponentUpd.h"
#include "System/Entity/EntityType.h"
#include "System/StageBounds.h"
#include <memory>
#include <vector>

namespace Atrum {

class Entity;

class CmpReflectAbleBullet : public ComponentUpd {
private:
  std::vector<std::unique_ptr<Entity>> reflectAbleBullets_{};

  EntityType reflectorType_ = EntityType::PLAYER_WEAPON;

  inline static constexpr float kSpaceTopBuffer = 8.0f;
  inline static constexpr float kSpaceSideBuffer = 8.0f;
  inline static constexpr float kSpaceBottomBuffer = 0.25f;

public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_RESPONSE;
  }

  void Initialize() override;
  void Update() override;

  /// 自機・ボス子实体の更新後に呼び、メテオ／壁弾などの当たりと演出を確定する
  void LateResolveCombatHits();

  void Shot(Entity *bullet);

  std::vector<std::unique_ptr<Entity>> &RefBullets() { return reflectAbleBullets_; }
};

} // namespace Atrum
