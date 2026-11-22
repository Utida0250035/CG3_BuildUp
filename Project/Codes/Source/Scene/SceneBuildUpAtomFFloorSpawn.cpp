#include "Scene/SceneBuildUpAtomFFloorSpawn.h"

#include "Math/Transform.h"
#include "System/AtomF/CmpAtomFArenaWall.h"
#include "System/AtomF/CmpAtomFFloor.h"
#include "System/Entity/Entity.h"

namespace Atrum {

namespace {

bool PathIsAtomFScene(const std::string &sceneJsonPath) {
  constexpr const char *kFileName = "atomF.json";
  constexpr std::size_t kFileNameLength = 10;
  if (sceneJsonPath.size() < kFileNameLength) {
    return false;
  }
  return sceneJsonPath.compare(sceneJsonPath.size() - kFileNameLength,
                               kFileNameLength, kFileName) == 0;
}

void AppendAtomFArenaWall(std::vector<std::unique_ptr<Entity>> &entities) {
  std::unique_ptr<Entity> wall = std::make_unique<Entity>("AtomFArenaWall");
  Math::TransformLH initialTransform{};
  initialTransform.translate = {0.0f, 0.0f, 0.0f};
  wall->SetInitialTransform(initialTransform);
  wall->AddDrawCmp<CmpAtomFArenaWall>();
  wall->Initialize();
  entities.push_back(std::move(wall));
}

} // namespace

void AppendAtomFFloorIfScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  if (!PathIsAtomFScene(sceneJsonPath)) {
    return;
  }

  std::unique_ptr<Entity> floor = std::make_unique<Entity>("AtomFFloor");
  Math::TransformLH initialTransform{};
  initialTransform.translate = {0.0f, 0.0f, 0.0f};
  floor->SetInitialTransform(initialTransform);
  floor->AddDrawCmp<CmpAtomFFloor>();
  floor->Initialize();
  entities.push_back(std::move(floor));
  AppendAtomFArenaWall(entities);
}

} // namespace Atrum
