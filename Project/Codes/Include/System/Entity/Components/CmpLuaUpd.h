#pragma once

#include "System/Entity/ComponentUpd.h"
#include <sol/forward.hpp>
#include <string>
#include <vector>

namespace Atrum {

/// Lua 更新コンポーネント（1 Entity に複数スクリプトを束ねる）
class CmpLuaUpd final : public ComponentUpd {
public:
  struct Binding final {
    std::string scriptPath{};
    UpdCmpGroup group = UpdCmpGroup::MOVEMENT;
    sol::table module{};
    bool moduleLoaded = false;
  };

  constexpr UpdCmpGroup UpdGroup() const override { return primaryGroup_; }

  void ResolveDependence() override;
  void Update() override;
  void Finalize() override;

  void AddOrUpdateBinding(const std::string &scriptPath,
                          std::string_view updCmpGroupName);

  const std::vector<Binding> &Bindings() const { return bindings_; }

private:
  void LoadBindingModule(Binding &binding);
  void InvokeResolveForBinding(Binding &binding);
  void InvokeFinalizeForBinding(const Binding &binding) const;

  std::vector<Binding> bindings_{};
  UpdCmpGroup primaryGroup_ = UpdCmpGroup::MOVEMENT;
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
};

} // namespace Atrum