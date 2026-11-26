#pragma once

#include "System/Scripting/ILuaBinder.h"

namespace Atrum::Script {

/// ゲームロジックを Lua で書くための基本 API を公開する。
///   Input.anyTrigger() / confirmTrigger() / upTrigger() / downTrigger() / stickY()
///   SceneFlow.change(sceneJsonPath) / isChanging() / countByRole(role) / healthByRole(role)
///   Ui.image(texturePath, x, y [, {scale=, r=, g=, b=, a=}])
class EngineLuaBinder final : public ILuaBinder {
public:
  void Register(sol::state &lua) override;
};

} // namespace Atrum::Script
