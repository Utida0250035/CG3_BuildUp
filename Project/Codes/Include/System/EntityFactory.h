#pragma once

#include "Math/Vector3.h"
#include <memory>
#include <vector>

namespace Atrum {

class Camera;
class Entity;

class EntityFactory final {
private:
  EntityFactory() = delete;

public:
  static std::unique_ptr<Entity> PlayerParry();
  static std::unique_ptr<Entity> EnemyAstro(Entity *player);
  static std::vector<std::unique_ptr<Entity>> ScrollSpheres();
  /// 画面下の床際を隠す白い雲（自機の足元が少し見えなくなる程度）
  static std::vector<std::unique_ptr<Entity>> ForegroundGroundClouds();
  static std::unique_ptr<Entity> SkyDome(Camera *camera);
  static std::unique_ptr<Entity> LinearEnemyBullet(const Math::Vector3 &velocity,
                                                   const Math::Vector3 &position);
  static std::unique_ptr<Entity> Parry(Entity *parent);

  EntityFactory(const EntityFactory &) = delete;
  EntityFactory &operator=(const EntityFactory &) = delete;
};

} // namespace Atrum
