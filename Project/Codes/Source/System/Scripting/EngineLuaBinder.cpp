#include "System/Scripting/EngineLuaBinder.h"

#include "Input/PlayInput.h"
#include "Input/XInputController.h"
#include "Engine/EngineAccess.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Scripting/LuaSystem.h"
#include "System/Scripting/ScriptSceneContext.h"
#include "System/Scripting/ScriptUiSprite.h"

#include <string>

namespace Atrum::Script {

namespace {

namespace I = Atrum::Input;

constexpr int kScanCodeCount = 256;
/// SDL_SCANCODE_F1〜F12（F1 は BuildUp の復帰キーなど制作用に使うので「何かキー」から除外）
constexpr int kScanCodeF1 = 58;
constexpr int kScanCodeF12 = 69;
constexpr int kGamepadButtonCount = 12;

bool IsAnyInputTrigger() {
  if (I::PlayInput *const keyboard = ::Atrum::EnginePlayInput()) {
    for (int code = 0; code < kScanCodeCount; ++code) {
      if (code >= kScanCodeF1 && code <= kScanCodeF12) {
        continue;
      }
      if (keyboard->IsKeyTrigger(static_cast<I::Key>(code))) {
        return true;
      }
    }
  }

  I::XInputController *const pad = I::XInputController::Instance();
  if (pad != nullptr && pad->IsConnected()) {
    for (int i = 0; i < kGamepadButtonCount; ++i) {
      if (pad->IsButtonTrigger(static_cast<I::GamepadButton>(i))) {
        return true;
      }
    }
  }
  return false;
}

/// 決定: Space / Enter / パッド A
bool IsConfirmTrigger() {
  if (I::PlayInput *const keyboard = ::Atrum::EnginePlayInput()) {
    if (keyboard->IsKeyTrigger(I::Key::SPACE) ||
        keyboard->IsKeyTrigger(I::Key::RETURN)) {
      return true;
    }
  }
  I::XInputController *const pad = I::XInputController::Instance();
  return pad != nullptr && pad->IsConnected() &&
         pad->IsButtonTrigger(I::GamepadButton::A);
}

bool IsDirectionTrigger(const bool up) {
  if (I::PlayInput *const keyboard = ::Atrum::EnginePlayInput()) {
    const bool key = up ? (keyboard->IsKeyTrigger(I::Key::W) ||
                           keyboard->IsKeyTrigger(I::Key::UP))
                        : (keyboard->IsKeyTrigger(I::Key::S) ||
                           keyboard->IsKeyTrigger(I::Key::DOWN));
    if (key) {
      return true;
    }
  }
  I::XInputController *const pad = I::XInputController::Instance();
  if (pad == nullptr || !pad->IsConnected()) {
    return false;
  }
  return pad->IsButtonTrigger(up ? I::GamepadButton::DpadUp
                                 : I::GamepadButton::DpadDown);
}

float LeftStickY() {
  I::XInputController *const pad = I::XInputController::Instance();
  if (pad == nullptr || !pad->IsConnected()) {
    return 0.0f;
  }
  return pad->GetLeftStick().y;
}

bool ChangeToBuildUp(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty() ||
      CommandChangeScene::Instance()->GetIsExecute()) {
    return false;
  }
  SceneBuildUpLaunchPolicy::QueueFadeToBuildUp(sceneJsonPath,
                                               SceneContext::ReturnScene());
  return true;
}

bool IsChangingScene() { return CommandChangeScene::Instance()->GetIsExecute(); }

int CountActiveByRole(const std::string &role) {
  const std::vector<std::unique_ptr<Entity>> *const entities =
      SceneContext::Entities();
  if (entities == nullptr) {
    return 0;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  int count = 0;
  for (const std::unique_ptr<Entity> &entity : *entities) {
    if (entity != nullptr && entity->IsActive() &&
        tags.EntityHasRole(*entity, role)) {
      ++count;
    }
  }
  return count;
}

/// role を持つ最初の有効エンティティの CmpHealthHit の体力。該当なしは -1。
int HealthOfFirstByRole(const std::string &role) {
  const std::vector<std::unique_ptr<Entity>> *const entities =
      SceneContext::Entities();
  if (entities == nullptr) {
    return -1;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  for (const std::unique_ptr<Entity> &entity : *entities) {
    if (entity == nullptr || !entity->IsActive() ||
        !tags.EntityHasRole(*entity, role)) {
      continue;
    }
    const CmpHealthHit *const health = entity->GetUpdCmp<CmpHealthHit>();
    if (health == nullptr) {
      continue;
    }
    return static_cast<int>(health->GetHealthCount());
  }
  return -1;
}

/// ボスの Hp。ボスが居なければ -1（撃破演出中は 0）。
int BossHealth() {
  const CmpAtomFBoss *const boss = CmpAtomFBoss::Active();
  return boss != nullptr ? boss->Health() : -1;
}

void PushUiImage(const std::string &path, const float x, const float y,
                 const sol::optional<sol::table> options) {
  UiSprite::Entry entry{};
  entry.texturePath = path;
  entry.x = x;
  entry.y = y;
  if (options) {
    const sol::table &t = *options;
    entry.scale = t.get_or("scale", entry.scale);
    entry.r = t.get_or("r", entry.r);
    entry.g = t.get_or("g", entry.g);
    entry.b = t.get_or("b", entry.b);
    entry.a = t.get_or("a", entry.a);
  }
  UiSprite::Push(entry);
}

} // namespace

void EngineLuaBinder::Register(sol::state &lua) {
  sol::table input = lua.create_named_table("Input");
  input.set_function("anyTrigger", &IsAnyInputTrigger);
  input.set_function("confirmTrigger", &IsConfirmTrigger);
  input.set_function("upTrigger", []() { return IsDirectionTrigger(true); });
  input.set_function("downTrigger", []() { return IsDirectionTrigger(false); });
  input.set_function("stickY", &LeftStickY);

  sol::table sceneFlow = lua.create_named_table("SceneFlow");
  sceneFlow.set_function("change", &ChangeToBuildUp);
  sceneFlow.set_function("isChanging", &IsChangingScene);
  sceneFlow.set_function("countByRole", &CountActiveByRole);
  sceneFlow.set_function("healthByRole", &HealthOfFirstByRole);
  // atomF のステージ進行用: 雑魚（変種名・位置 XZ）とボスの出現、ボス Hp。
  sceneFlow.set_function("spawnEnemy", &EnqueueAtomFEnemy);
  sceneFlow.set_function("spawnBoss", &EnqueueAtomFBoss);
  sceneFlow.set_function("bossHealth", &BossHealth);

  sol::table ui = lua.create_named_table("Ui");
  ui.set_function("image", &PushUiImage);
}

} // namespace Atrum::Script
