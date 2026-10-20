#include "System/Scripting/LuaSystemPool.h"
#include "System/Hash64.h"
#include "System/Scripting/LuaSystem.h"

namespace Atrum::Script {

void LuaSystemPool::Add(const std::string &name) {
  map_.emplace(Hash64(name), luaSystems_.size());
  luaSystems_.emplace_back(std::make_unique<LuaSystem>());
}

bool LuaSystemPool::InitSystems() {
  for (auto &system : luaSystems_) {
    if (!system->Initialize()) {

      return false;
    }
  }

  return true;
}

LuaSystem *LuaSystemPool::Find(const std::string &name) {

  auto search = map_.find(Hash64(name));

  if (search != map_.end()) {
    return luaSystems_[search->second].get();
  }

  return nullptr;
}

} // namespace Atrum::Script