#include "System/Scripting/AtomFLuaBinder.h"

#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Entity.h"
#include "System/Scripting/LuaSystem.h"
#include "System/Scripting/ScriptSceneContext.h"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace Atrum::Script {

namespace {

CmpAtomFBoss *FindBoss() {
  const std::vector<std::unique_ptr<Entity>> *const entities =
      SceneContext::Entities();
  if (entities == nullptr) {
    return nullptr;
  }
  for (const std::unique_ptr<Entity> &entity : *entities) {
    if (entity == nullptr || !entity->IsActive()) {
      continue;
    }
    if (entity->HasUpdCmp<CmpAtomFBoss>()) {
      return entity->GetUpdCmp<CmpAtomFBoss>();
    }
  }
  return nullptr;
}

int VariantCount() { return AtomF::EnemyVariantCount(); }

int VariantIndex(const std::string &name) {
  return AtomF::FindEnemyVariantIndex(name);
}

bool SpawnEnemy(const sol::object &variant, const float x, const float z) {
  int index = 0;
  if (variant.is<int>()) {
    index = variant.as<int>();
  } else if (variant.is<std::string>()) {
    index = AtomF::FindEnemyVariantIndex(variant.as<std::string>());
    if (index < 0) {
      index = 0;
    }
  }
  return AtomF::SpawnEnemyAtWorld(index, x, z);
}

bool SpawnBoss(const float x, const float z) {
  return AtomF::SpawnBossAtWorld(x, z);
}

int BossHealth() {
  const CmpAtomFBoss *boss = FindBoss();
  return boss != nullptr ? boss->GetHealth() : -1;
}

int BossMaxHealth() {
  const CmpAtomFBoss *boss = FindBoss();
  return boss != nullptr ? boss->GetMaxHealth()
                         : AtomF::ResolveBossParameters().maxHealth;
}

std::string BossState() {
  const CmpAtomFBoss *boss = FindBoss();
  if (boss == nullptr) {
    return std::string{};
  }
  switch (boss->GetState()) {
  case CmpAtomFBoss::State::Appearing:
    return "appearing";
  case CmpAtomFBoss::State::Walking:
    return "walking";
  case CmpAtomFBoss::State::Telegraph:
    return "telegraph";
  case CmpAtomFBoss::State::Executing:
    return "executing";
  case CmpAtomFBoss::State::Staggered:
    return "staggered";
  case CmpAtomFBoss::State::Roaring:
    return "roaring";
  case CmpAtomFBoss::State::Dying:
    return "dying";
  }
  return std::string{};
}

std::tuple<float, float> PlayerPosition() {
  const Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return {0.0f, 0.0f};
  }
  const Math::Vector3 pos = player->GetWorldPosition();
  return {pos.x, pos.z};
}

float ArenaRadius() { return AtomF::ResolveArenaInnerRadiusMeters(); }

} // namespace

void AtomFLuaBinder::Register(sol::state &lua) {
  sol::table atomF = lua.create_named_table("AtomF");
  atomF.set_function("variantCount", &VariantCount);
  atomF.set_function("variantIndex", &VariantIndex);
  atomF.set_function("spawnEnemy", &SpawnEnemy);
  atomF.set_function("spawnBoss", &SpawnBoss);
  atomF.set_function("bossHealth", &BossHealth);
  atomF.set_function("bossMaxHealth", &BossMaxHealth);
  atomF.set_function("bossState", &BossState);
  atomF.set_function("playerPosition", &PlayerPosition);
  atomF.set_function("arenaRadius", &ArenaRadius);
}

} // namespace Atrum::Script
