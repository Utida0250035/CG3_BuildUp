#include "System/Scripting/LuaEntityApiBinder.h"

#include "System/Entity/EntityTagRegistry.h"
#include "System/Scripting/LuaComponentModuleLoad.h"
#include "System/Scripting/LuaEntityComponentAttach.h"
#include "System/Scripting/LuaEntitySpawnFromJson.h"

#include <iostream>
#include <string>

namespace Atrum::Script {

LuaEntityApiBinder::LuaEntityApiBinder(const LuaEntityApiContext context)
    : context_(context) {}

void LuaEntityApiBinder::Register(sol::state &lua) {
  sol::table atrum = lua.create_named_table("Atrum");

  atrum.set_function("log", [](const std::string &message) {
    std::cerr << "[Lua] " << message << std::endl;
  });

  atrum.set_function("tag_for_role",
                     [](const std::string &roleKey) -> std::string {
                       EntityTagRegistry::Instance().EnsureLoaded();
                       return EntityTagRegistry::Instance().TagNameForRole(
                           roleKey);
                     });

  atrum.set_function(
      "spawn_entity_from_json",
      [&lua, this](const std::string &jsonPath, sol::optional<float> offsetX,
                   sol::optional<float> offsetY,
                   sol::optional<float> offsetZ) -> sol::table {
        sol::table response = lua.create_table();
        if (context_.entities == nullptr) {
          response["ok"] = false;
          response["error"] = "entities context is not bound";
          response["count"] = 0;
          return response;
        }

        const LuaEntitySpawnResult spawnResult =
            TryAppendEntitiesFromJsonPath(
                jsonPath, *context_.entities, offsetX.value_or(0.0f),
                offsetY.value_or(0.0f), offsetZ.value_or(0.0f));

        response["ok"] = spawnResult.success;
        response["count"] = spawnResult.spawnedCount;
        if (spawnResult.success) {
          response["message"] = spawnResult.message;
        } else {
          response["error"] = spawnResult.message;
        }
        return response;
      });

  atrum.set_function(
      "attach_lua_upd",
      [&lua, this](const std::string &entityName, const std::string &scriptPath,
                   sol::optional<std::string> updCmpGroup) -> sol::table {
        sol::table response = lua.create_table();
        if (context_.entities == nullptr) {
          response["ok"] = false;
          response["error"] = "entities context is not bound";
          return response;
        }
        const LuaComponentAttachResult attach =
            TryAttachLuaUpdByEntityName(
                *context_.entities, entityName, scriptPath,
                updCmpGroup.value_or("MOVEMENT"));
        response["ok"] = attach.success;
        response[attach.success ? "message" : "error"] = attach.message;
        return response;
      });

  atrum.set_function(
      "attach_lua_draw",
      [&lua, this](const std::string &entityName, const std::string &scriptPath,
                   sol::optional<std::string> drawCmpGroup) -> sol::table {
        sol::table response = lua.create_table();
        if (context_.entities == nullptr) {
          response["ok"] = false;
          response["error"] = "entities context is not bound";
          return response;
        }
        const LuaComponentAttachResult attach =
            TryAttachLuaDrawByEntityName(
                *context_.entities, entityName, scriptPath,
                drawCmpGroup.value_or("NONE"));
        response["ok"] = attach.success;
        response[attach.success ? "message" : "error"] = attach.message;
        return response;
      });

  if (context_.tickCallbacks != nullptr) {
    atrum.set_function(
        "require_behavior",
        [&lua, this](const std::string &scriptPath) -> sol::table {
          sol::table response = lua.create_table();
          const std::string resolved =
              ResolveLuaScriptPathForComponent(scriptPath);
          sol::protected_function_result loadResult =
              lua.safe_script_file(resolved, sol::script_pass_on_error);
          if (!loadResult.valid()) {
            sol::error err = loadResult;
            response["ok"] = false;
            response["error"] = err.what();
            return response;
          }

          sol::object loaded = loadResult;
          sol::table module{};
          if (loaded.is<sol::table>()) {
            module = loaded;
          } else {
            response["ok"] = false;
            response["error"] = "behavior must return a table";
            return response;
          }

          sol::optional<sol::protected_function> onTick = module["onTick"];
          if (onTick && context_.tickCallbacks != nullptr) {
            context_.tickCallbacks->push_back(*onTick);
          }

          response["ok"] = true;
          response["path"] = resolved;
          response["has_onTick"] = static_cast<bool>(onTick);
          return response;
        });
  }
}

} // namespace Atrum::Script