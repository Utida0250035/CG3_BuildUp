#include "System/Scripting/AtomFLuaBinder.h"

#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Entity.h"
#include "System/Scripting/LuaSystem.h"

#include <string>
#include <tuple>

namespace Atrum::Script {

namespace {

sol::table VariantNames(sol::this_state state) {
  sol::state_view lua(state);
  sol::table names = lua.create_table();
  const AtomF::ParametersRuntime &runtime = AtomF::ParametersRuntime::Instance();
  if (!runtime.IsLoaded()) {
    return names;
  }
  int index = 1;
  for (const AtomF::EnemyVariantParameters &variant :
       runtime.Enemy().variants) {
    names[index++] = variant.name;
  }
  return names;
}

float ArenaRadius() { return AtomF::ResolveArenaInnerRadiusMeters(); }

std::tuple<sol::object, sol::object> PlayerPosition(sol::this_state state) {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return std::make_tuple(sol::make_object(state, sol::lua_nil),
                           sol::make_object(state, sol::lua_nil));
  }
  const Math::Vector3 position = player->GetWorldPosition();
  return std::make_tuple(sol::make_object(state, position.x),
                         sol::make_object(state, position.z));
}

int BossHealth() {
  const CmpAtomFBoss *boss = CmpAtomFBoss::Active();
  return boss != nullptr ? boss->Health() : -1;
}

int BossMaxHealth() { return AtomF::ResolveBossParameters().maxHealth; }

bool BossPhase2() {
  const CmpAtomFBoss *boss = CmpAtomFBoss::Active();
  return boss != nullptr && boss->IsPhase2();
}

std::string BossState() {
  const CmpAtomFBoss *boss = CmpAtomFBoss::Active();
  if (boss == nullptr) {
    return "none";
  }
  switch (boss->CurrentState()) {
  case CmpAtomFBoss::State::Appear:
    return "appear";
  case CmpAtomFBoss::State::Stunned:
    return "stunned";
  case CmpAtomFBoss::State::Dying:
    return "dying";
  default:
    return "fight";
  }
}

} // namespace

void AtomFLuaBinder::Register(sol::state &lua) {
  sol::table atomF = lua.create_named_table("AtomF");
  atomF.set_function("spawnEnemy", &EnqueueAtomFEnemy);
  atomF.set_function("spawnBoss", &EnqueueAtomFBoss);
  atomF.set_function("variantNames", &VariantNames);
  atomF.set_function("arenaRadius", &ArenaRadius);
  atomF.set_function("playerPosition", &PlayerPosition);
  atomF.set_function("bossHealth", &BossHealth);
  atomF.set_function("bossMaxHealth", &BossMaxHealth);
  atomF.set_function("bossPhase2", &BossPhase2);
  atomF.set_function("bossState", &BossState);
}

} // namespace Atrum::Script
