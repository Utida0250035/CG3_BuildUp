#pragma once

#include <memory>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// atomF ランタイムで player 役エンティティを1体指す（シーン tick が更新）。
class ScenePlayerLocator final {
public:
  static ScenePlayerLocator &Instance();

  void SetPlayerEntity(Entity *player);
  Entity *PlayerEntity() const { return player_; }

private:
  Entity *player_ = nullptr;
};

void RefreshScenePlayerLocatorFromEntities(
    const std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum::AtomF