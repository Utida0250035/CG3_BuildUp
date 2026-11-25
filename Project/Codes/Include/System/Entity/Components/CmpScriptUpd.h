#pragma once

#include "System/Entity/ComponentUpd.h"

#include <cstdint>
#include <string>

namespace Atrum {

namespace Script {
class LuaJitScriptHost;
}

class CmpScriptUpd : public ComponentUpd {
private:
  Script::LuaJitScriptHost *host_ = nullptr;
  std::string scriptPath_;
  bool disabled_ = false;
  uint64_t entityKey_ = 0;
  UpdCmpGroup updGroup_ = UpdCmpGroup::MOVEMENT;
  uint64_t updSortPriority_ = static_cast<uint64_t>(UpdCmpGroup::MOVEMENT);

public:
  UpdCmpGroup UpdGroup() const override { return updGroup_; }

  void ResolveDependence() override;
  void Update() override;
  void Finalize() override;

  void SetScriptPath(const std::string &path) { scriptPath_ = path; }
  const std::string &GetScriptPath() const { return scriptPath_; }

  void ConfigureUpdGroup(bool useNumericPriority, UpdCmpGroup enumGroup,
                         uint64_t numericPriority);
  void SyncUpdSortPriorityToEntity();

  bool IsDisabled() const { return disabled_; }

  void DevelopGui() override;
};

} // namespace Atrum