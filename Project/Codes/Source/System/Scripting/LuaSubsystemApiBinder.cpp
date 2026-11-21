#include "System/Scripting/LuaSubsystemApiBinder.h"

#include "Audio/Audio.h"
#include "Engine/Command/CommandContext.h"
#include "Engine/EngineAccess.h"
#include "Engine/Resource/AssetModel.h"
#include "Engine/Resource/ModelStorage.h"
#include "Engine/Resource/TextureStorage.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#include "Scene/SceneBuildUpRegistry.h"
#include "Scene/SceneMovie.h"
#include "Scene/SceneMovieRegistry.h"
#include "System/Editor/Entity/EntityEditorKnownComponentRegistry.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityStorage.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/PlaySe.h"
#include "System/PlaySeDetailCache.h"
#include "System/Registry/RegistryCmp.h"
#include "System/Scripting/LuaEntityComponentAttach.h"

#include <sol/sol.hpp>
#include <cstdint>
#include <memory>
#include <string>

namespace Atrum::Script {

namespace {

Entity *ResolveEntityByName(LuaEntityApiContext &context,
                            const std::string &entityName) {
  if (context.entities != nullptr) {
    if (Entity *fromScene =
            FindEntityByNameHash(*context.entities, entityName)) {
      return fromScene;
    }
  }
  const EntityHandle handle = EntityStorage::Instance()->Find(entityName);
  return handle.ptr_;
}

sol::table AudioHandleToTable(sol::state &lua, const Audio::Handle &handle) {
  sol::table table = lua.create_table();
  table["soundIndex"] = handle.soundIndex;
  table["voiceIndex"] = handle.voiceIndex;
  table["isStreaming"] = handle.isStreaming;
  table["playId"] = handle.playId;
  table["isSuccess"] = handle.isSuccess;
  table["ok"] = static_cast<bool>(handle);
  return table;
}

Audio::Handle AudioHandleFromTable(const sol::table &table) {
  Audio::Handle handle{};
  if (!table.valid()) {
    return handle;
  }
  handle.soundIndex = table.get_or("soundIndex", static_cast<size_t>(0));
  handle.voiceIndex = table.get_or("voiceIndex", static_cast<size_t>(0));
  handle.isStreaming = table.get_or("isStreaming", false);
  handle.playId = table.get_or("playId", static_cast<uint64_t>(0));
  handle.isSuccess = table.get_or("isSuccess", false);
  return handle;
}

} // namespace

LuaSubsystemApiBinder::LuaSubsystemApiBinder(const LuaEntityApiContext context)
    : context_(context) {}

void LuaSubsystemApiBinder::Register(sol::state &lua) {
  sol::table atrum = lua["Atrum"];

  sol::table command = lua.create_table();
  sol::table commandScene = lua.create_table();
  commandScene.set_function("has_current_scene", []() -> bool {
    return CommandScene::Instance()->GetSceneNow() != nullptr;
  });
  command["Scene"] = commandScene;

  sol::table commandChange = lua.create_table();
  commandChange.set_function("is_executing", []() -> bool {
    return CommandChangeScene::Instance()->GetIsExecute();
  });
  commandChange.set_function("allows_current_scene_update", []() -> bool {
    return CommandChangeScene::Instance()->AllowsCurrentSceneUpdate();
  });
  commandChange.set_function("should_defer_enter_scene_bgm", []() -> bool {
    return CommandChangeScene::Instance()->ShouldDeferEnterSceneBgm();
  });
  commandChange.set_function(
      "queue_buildup_scene",
      [&lua](const std::string &sceneJsonPath) -> sol::table {
        sol::table response = lua.create_table();
        Scene *const current = CommandScene::Instance()->GetSceneNow();
        if (current == nullptr) {
          response["ok"] = false;
          response["error"] = "no current scene";
          return response;
        }
        SceneBuildUpLaunchPolicy::QueueFadeToBuildUp(sceneJsonPath, current);
        response["ok"] = true;
        response["sceneJsonPath"] = sceneJsonPath;
        return response;
      });
  commandChange.set_function(
      "queue_movie_scene",
      [&lua](const std::string &trackJsonPath) -> sol::table {
        sol::table response = lua.create_table();
        Scene *const current = CommandScene::Instance()->GetSceneNow();
        if (current == nullptr) {
          response["ok"] = false;
          response["error"] = "no current scene";
          return response;
        }
        Scene *const next =
            SceneMovieRegistry::Instance().Launch(trackJsonPath, current);
        if (next == nullptr) {
          response["ok"] = false;
          response["error"] = "SceneMovieRegistry launch failed";
          return response;
        }
        CommandChangeScene::Instance()->Set(
            next, std::make_unique<ChangeSceneFade>().release());
        response["ok"] = true;
        response["trackJsonPath"] = trackJsonPath;
        return response;
      });
  command["ChangeScene"] = commandChange;

  sol::table commandContext = lua.create_table();
  commandContext.set_function("list_type", []() -> int {
    CommandContext *ctx = EngineCommandContextDirect();
    if (ctx == nullptr) {
      return -1;
    }
    return static_cast<int>(ctx->GetType());
  });
  commandContext.set_function("is_ready", []() -> bool {
    return EngineCommandContextDirect() != nullptr;
  });
  command["Context"] = commandContext;

  atrum["Command"] = command;

  sol::table audio = lua.create_table();
  audio.set_function(
      "load",
      [&lua](const std::string &path) -> sol::table {
        sol::table response = lua.create_table();
        Audio::AudioManager *manager = EngineAudioManager();
        if (manager == nullptr) {
          response["ok"] = false;
          response["error"] = "AudioManager unavailable";
          return response;
        }
        const size_t index = manager->Load(path);
        response["ok"] = index != static_cast<size_t>(-1);
        response["soundIndex"] = index;
        return response;
      });
  audio.set_function(
      "get_index",
      [&lua](const std::string &path) -> sol::table {
        sol::table response = lua.create_table();
        const size_t index = PlaySeDetail::GetOrLoadSeIndex(path);
        response["ok"] = index != static_cast<size_t>(-1);
        response["soundIndex"] = index;
        return response;
      });
  audio.set_function(
      "play",
      [&lua](const std::string &path,
             sol::optional<bool> loop) -> sol::table {
        const Audio::Handle handle =
            loop.value_or(false) ? PlaySeLoop(path) : PlaySe(path);
        sol::table response = AudioHandleToTable(lua, handle);
        return response;
      });
  audio.set_function(
      "stop", [](const sol::table &handleTable) -> bool {
        StopSe(AudioHandleFromTable(handleTable));
        return true;
      });
  audio.set_function("is_playing", [](const sol::table &handleTable) -> bool {
    Audio::AudioManager *manager = EngineAudioManager();
    if (manager == nullptr) {
      return false;
    }
    return manager->IsPlaying(AudioHandleFromTable(handleTable));
  });
  audio.set_function(
      "set_volume",
      [](const sol::table &handleTable, const float volume) -> bool {
        return SetHandleVolume(AudioHandleFromTable(handleTable), volume);
      });
  audio.set_function(
      "get_volume",
      [&lua](const sol::table &handleTable) -> sol::table {
        sol::table response = lua.create_table();
        float volume = 0.0f;
        const bool ok =
            GetHandleVolume(AudioHandleFromTable(handleTable), volume);
        response["ok"] = ok;
        response["volume"] = volume;
        return response;
      });
  audio.set_function(
      "play_scene_bgm",
      [&lua](const std::string &path) -> sol::table {
        const size_t index = PlaySeDetail::GetOrLoadSeIndex(path);
        if (index == static_cast<size_t>(-1)) {
          sol::table response = lua.create_table();
          response["ok"] = false;
          response["error"] = "load failed";
          return response;
        }
        return AudioHandleToTable(lua, PlaySceneBgm(index));
      });
  atrum["Audio"] = audio;

  sol::table storage = lua.create_table();
  sol::table entityStorage = lua.create_table();
  entityStorage.set_function("find", [&lua](const std::string &entityName) -> sol::table {
        sol::table response = lua.create_table();
        const EntityHandle handle =
            EntityStorage::Instance()->Find(entityName);
        response["ok"] = handle.ptr_ != nullptr;
        response["entityId"] = handle.id_;
        response["valid"] = handle.IsValid();
        return response;
      });
  entityStorage.set_function("find_in_scene", [this, &lua](const std::string &entityName)
                                 -> sol::table {
    sol::table response = lua.create_table();
    Entity *entity = ResolveEntityByName(context_, entityName);
    response["ok"] = entity != nullptr;
    if (entity != nullptr) {
      response["entityId"] = entity->GetId();
      response["active"] = entity->IsActive();
    }
    return response;
  });
  storage["Entity"] = entityStorage;

  sol::table textureStorage = lua.create_table();
  textureStorage.set_function(
      "get_index",
      [&lua](const std::string &filePath) -> sol::table {
        sol::table response = lua.create_table();
        TextureStorage *textures = EngineTextureStorage();
        if (textures == nullptr) {
          response["ok"] = false;
          response["error"] = "TextureStorage unavailable";
          return response;
        }
        const uint32_t index = textures->GetTexture(filePath);
        response["ok"] = true;
        response["textureIndex"] = index;
        response["width"] = textures->GetTextureWidth(index);
        response["height"] = textures->GetTextureHeight(index);
        return response;
      });
  textureStorage.set_function(
      "get_dimensions",
      [&lua](const uint32_t textureIndex) -> sol::table {
        sol::table response = lua.create_table();
        TextureStorage *textures = EngineTextureStorage();
        if (textures == nullptr) {
          response["ok"] = false;
          return response;
        }
        response["ok"] = true;
        response["width"] = textures->GetTextureWidth(textureIndex);
        response["height"] = textures->GetTextureHeight(textureIndex);
        return response;
      });
  storage["Texture"] = textureStorage;

  sol::table modelStorage = lua.create_table();
  modelStorage.set_function(
      "get",
      [&lua](const std::string &directoryPathObj, const std::string &objFileName,
             const std::string &directoryPathMtl,
             const std::string &mtlFileName) -> sol::table {
        sol::table response = lua.create_table();
        const std::shared_ptr<AssetModel> model = EngineGetModel(
            directoryPathObj, objFileName, directoryPathMtl, mtlFileName);
        response["ok"] = model != nullptr;
        response["loaded"] = static_cast<bool>(model);
        return response;
      });
  modelStorage.set_function("is_ready", []() -> bool {
    return EngineModelStorage() != nullptr;
  });
  storage["Model"] = modelStorage;

  atrum["Storage"] = storage;

  sol::table registry = lua.create_table();
  sol::table entityTagRegistry = lua.create_table();
  entityTagRegistry.set_function(
      "tag_for_role",
      [](const std::string &roleKey) -> std::string {
        EntityTagRegistry::Instance().EnsureLoaded();
        return EntityTagRegistry::Instance().TagNameForRole(roleKey);
      });
  entityTagRegistry.set_function("reload", [](const std::string &jsonPath) -> bool {
    return EntityTagRegistry::Instance().ReloadFromJsonPath(jsonPath);
  });
  entityTagRegistry.set_function("hash_for_role", [](const std::string &roleKey) -> uint64_t {
    EntityTagRegistry::Instance().EnsureLoaded();
    return EntityTagRegistry::Instance().HashForRole(roleKey);
  });
  registry["EntityTag"] = entityTagRegistry;

  sol::table registryCmp = lua.create_table();
  registryCmp.set_function("upd_count", []() -> size_t {
    return RegistryCmp::Instance()->UpdRegistrationCount();
  });
  registryCmp.set_function("draw_count", []() -> size_t {
    return RegistryCmp::Instance()->DrawRegistrationCount();
  });
  registryCmp.set_function("data_count", []() -> size_t {
    return RegistryCmp::Instance()->DataRegistrationCount();
  });
  registryCmp.set_function(
      "has_upd", [](const std::string &cmpName) -> bool {
        return RegistryCmp::Instance()->HasUpdRegistration(cmpName);
      });
  registryCmp.set_function(
      "has_draw", [](const std::string &cmpName) -> bool {
        return RegistryCmp::Instance()->HasDrawRegistration(cmpName);
      });
  registryCmp.set_function(
      "has_data", [](const std::string &cmpName) -> bool {
        return RegistryCmp::Instance()->HasDataRegistration(cmpName);
      });
  registryCmp.set_function(
      "give_upd",
      [this, &lua](const std::string &entityName, const std::string &cmpName) -> sol::table {
        sol::table response = lua.create_table();
        Entity *entity = ResolveEntityByName(context_, entityName);
        if (entity == nullptr) {
          response["ok"] = false;
          response["error"] = "entity not found";
          return response;
        }
        if (!RegistryCmp::Instance()->HasUpdRegistration(cmpName)) {
          response["ok"] = false;
          response["error"] = "unknown upd component: " + cmpName;
          return response;
        }
        RegistryCmp::Instance()->GiveCmpUpd(cmpName, entity);
        response["ok"] = true;
        return response;
      });
  registryCmp.set_function(
      "give_draw",
      [this, &lua](const std::string &entityName, const std::string &cmpName) -> sol::table {
        sol::table response = lua.create_table();
        Entity *entity = ResolveEntityByName(context_, entityName);
        if (entity == nullptr) {
          response["ok"] = false;
          response["error"] = "entity not found";
          return response;
        }
        if (!RegistryCmp::Instance()->HasDrawRegistration(cmpName)) {
          response["ok"] = false;
          response["error"] = "unknown draw component: " + cmpName;
          return response;
        }
        RegistryCmp::Instance()->GiveCmpDraw(cmpName, entity);
        response["ok"] = true;
        return response;
      });
  registryCmp.set_function(
      "give_data",
      [this, &lua](const std::string &entityName, const std::string &cmpName) -> sol::table {
        sol::table response = lua.create_table();
        Entity *entity = ResolveEntityByName(context_, entityName);
        if (entity == nullptr) {
          response["ok"] = false;
          response["error"] = "entity not found";
          return response;
        }
        if (!RegistryCmp::Instance()->HasDataRegistration(cmpName)) {
          response["ok"] = false;
          response["error"] = "unknown data component: " + cmpName;
          return response;
        }
        RegistryCmp::Instance()->GiveCmpData(cmpName, entity);
        response["ok"] = true;
        return response;
      });
  registry["Cmp"] = registryCmp;

  sol::table sceneBuildUpRegistry = lua.create_table();
  sceneBuildUpRegistry.set_function(
      "create_owned",
      [&lua](const std::string &sceneJsonPath) -> sol::table {
        sol::table response = lua.create_table();
        Scene *const current = CommandScene::Instance()->GetSceneNow();
        Scene *const scene = SceneBuildUpRegistry::Instance().Launch(
            sceneJsonPath, current);
        response["ok"] = scene != nullptr;
        response["created"] = scene != nullptr;
        return response;
      });
  registry["SceneBuildUp"] = sceneBuildUpRegistry;

  sol::table sceneMovieRegistry = lua.create_table();
  sceneMovieRegistry.set_function(
      "create_owned",
      [&lua](const std::string &trackJsonPath) -> sol::table {
        sol::table response = lua.create_table();
        Scene *const current = CommandScene::Instance()->GetSceneNow();
        Scene *const scene =
            SceneMovieRegistry::Instance().Launch(trackJsonPath, current);
        response["ok"] = scene != nullptr;
        return response;
      });
  registry["SceneMovie"] = sceneMovieRegistry;

  sol::table editorComponentRegistry = lua.create_table();
  editorComponentRegistry.set_function(
      "contains",
      [](const std::string &typeName) -> bool {
        return EntityEditor::EntityEditorKnownComponentRegistry::Contains(
            typeName);
      });
  registry["EditorComponent"] = editorComponentRegistry;

  atrum["Registry"] = registry;
}

} // namespace Atrum::Script