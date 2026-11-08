#include "System/Entity/EntityTagRegistry.h"

#include "System/Entity/Entity.h"
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum {

namespace {

std::vector<uint64_t> ToTagHashes(const std::vector<std::string> &tagNames) {
  std::vector<uint64_t> hashes;
  hashes.reserve(tagNames.size());
  for (const std::string &name : tagNames) {
    hashes.push_back(Hash64(name));
  }
  return hashes;
}

} // namespace

EntityTagRegistry &EntityTagRegistry::Instance() {
  static EntityTagRegistry instance;
  return instance;
}

void EntityTagRegistry::ApplyDefaults() {
  tagCatalog_ = {"PLAYER",       "ENEMY",        "PLAYER_WEAPON",
                 "ENEMY_WEAPON", "PLAYER_ITEM",  "ENEMY_ITEM"};
  rolesByKey_ = {{"player", "PLAYER"},
                   {"enemy", "ENEMY"},
                   {"playerWeapon", "PLAYER_WEAPON"},
                   {"enemyWeapon", "ENEMY_WEAPON"},
                   {"playerItem", "PLAYER_ITEM"},
                   {"enemyItem", "ENEMY_ITEM"}};
  roleTagNames_ = tagCatalog_;
  managedBulletTags_ = {"ENEMY_WEAPON", "PLAYER_ITEM"};
  reflectFromTag_ = "ENEMY_WEAPON";
  reflectToTag_ = "PLAYER_ITEM";
  playerDamageHurterTags_ = {"ENEMY", "ENEMY_WEAPON"};
  enemyDamageHurterTags_ = {"PLAYER_ITEM"};
  parryReflectorRoleKey_ = "playerWeapon";
}

void EntityTagRegistry::RebuildDerivedCaches() {
  roleTagNames_.clear();
  for (const std::string &name : tagCatalog_) {
    for (const auto &[roleKey, roleTag] : rolesByKey_) {
      (void)roleKey;
      if (roleTag == name) {
        roleTagNames_.push_back(name);
        break;
      }
    }
  }
}

void EntityTagRegistry::EnsureLoaded() {
  if (loaded_) {
    return;
  }
  ReloadFromJsonPath(kDefaultRegistryPath);
  loaded_ = true;
}

bool EntityTagRegistry::ReloadFromJsonPath(const std::string &jsonFilePath) {
  std::ifstream file(jsonFilePath);
  if (!file.is_open()) {
    ApplyDefaults();
    return false;
  }

  nlohmann::json json{};
  file >> json;

  const std::vector<std::string> catalog =
      json.value("tagCatalog", std::vector<std::string>{});
  if (catalog.empty()) {
    ApplyDefaults();
    return false;
  }

  tagCatalog_ = catalog;

  rolesByKey_.clear();
  const nlohmann::json &roles = json.value("roles", nlohmann::json::object());
  for (const auto &[roleKey, roleValue] : roles.items()) {
    if (!roleValue.is_string()) {
      continue;
    }
    rolesByKey_[roleKey] = roleValue.get<std::string>();
  }

  RebuildDerivedCaches();

  const nlohmann::json combat =
      json.value("combat", nlohmann::json::object());
  managedBulletTags_ =
      combat.value("managedBulletTags", std::vector<std::string>{});
  const nlohmann::json reflectParry =
      combat.value("reflectParry", nlohmann::json::object());
  reflectFromTag_ = reflectParry.value("fromTag", std::string{});
  reflectToTag_ = reflectParry.value("toTag", std::string{});
  playerDamageHurterTags_ = combat.value(
      "playerDamageHurterTags", std::vector<std::string>{});
  enemyDamageHurterTags_ =
      combat.value("enemyDamageHurterTags", std::vector<std::string>{});
  parryReflectorRoleKey_ =
      combat.value("parryReflectorRole", parryReflectorRoleKey_);

  if (managedBulletTags_.empty()) {
    managedBulletTags_ = {"ENEMY_WEAPON", "PLAYER_ITEM"};
  }
  if (reflectFromTag_.empty()) {
    reflectFromTag_ = "ENEMY_WEAPON";
    reflectToTag_ = "PLAYER_ITEM";
  }
  if (playerDamageHurterTags_.empty()) {
    playerDamageHurterTags_ = {"ENEMY", "ENEMY_WEAPON"};
  }
  if (enemyDamageHurterTags_.empty()) {
    enemyDamageHurterTags_ = {"PLAYER_ITEM"};
  }
  return true;
}

std::string EntityTagRegistry::TagNameForRole(const std::string &roleKey) const {
  const auto found = rolesByKey_.find(roleKey);
  if (found == rolesByKey_.end()) {
    return {};
  }
  return found->second;
}

uint64_t EntityTagRegistry::HashForRole(const std::string &roleKey) const {
  const std::string tagName = TagNameForRole(roleKey);
  if (tagName.empty()) {
    return 0;
  }
  return Hash64(tagName);
}

bool EntityTagRegistry::EntityHasTagName(const Entity &entity,
                                         const std::string &tagName) const {
  return entity.HasTagHash(Hash64(tagName));
}

bool EntityTagRegistry::EntityHasRole(const Entity &entity,
                                      const std::string &roleKey) const {
  const std::string tagName = TagNameForRole(roleKey);
  if (tagName.empty()) {
    return false;
  }
  return EntityHasTagName(entity, tagName);
}

bool EntityTagRegistry::EntityHasManagedBulletRole(const Entity &entity) const {
  for (const std::string &tagName : managedBulletTags_) {
    if (EntityHasTagName(entity, tagName)) {
      return true;
    }
  }
  return false;
}

void EntityTagRegistry::ApplyReflectParryTagConversion(Entity &entity) const {
  if (!reflectFromTag_.empty()) {
    entity.RemoveTag(reflectFromTag_);
  }
  if (!reflectToTag_.empty()) {
    entity.AddTag(reflectToTag_);
  }
}

std::vector<uint64_t> EntityTagRegistry::PlayerDamageHurterTagHashes() const {
  return ToTagHashes(playerDamageHurterTags_);
}

std::vector<uint64_t> EntityTagRegistry::EnemyDamageHurterTagHashes() const {
  return ToTagHashes(enemyDamageHurterTags_);
}

uint64_t EntityTagRegistry::DefaultParryReflectorTagHash() const {
  return HashForRole(parryReflectorRoleKey_);
}

} // namespace Atrum
