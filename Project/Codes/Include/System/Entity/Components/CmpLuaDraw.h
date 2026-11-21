#pragma once

#include "System/Entity/ComponentDraw.h"
#include <sol/forward.hpp>
#include <string>
#include <vector>

namespace Atrum {

/// Lua 描画コンポーネント
class CmpLuaDraw final : public ComponentDraw {
public:
  struct Binding final {
    std::string scriptPath{};
    DrawCmpGroup group = DrawCmpGroup::NONE;
    sol::table module{};
    bool moduleLoaded = false;
  };

  constexpr DrawCmpGroup DrawGroup() const override { return primaryGroup_; }

  void ResolveDependence() override;
  void Draw() override;
  void Finalize() override;

  void AddOrUpdateBinding(const std::string &scriptPath,
                          std::string_view drawCmpGroupName);

private:
  void LoadBindingModule(Binding &binding);
  void InvokeResolveForBinding(Binding &binding);
  void InvokeFinalizeForBinding(const Binding &binding) const;

  std::vector<Binding> bindings_{};
  DrawCmpGroup primaryGroup_ = DrawCmpGroup::NONE;
};

} // namespace Atrum