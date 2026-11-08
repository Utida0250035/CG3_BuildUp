#pragma once

#include "System/GameAssetsPaths.h"
#include "System/Hash64.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Atrum {

class Entity;

/// タグ名・戦闘用タグ対応を JSON で保持（定数タグは持たない）
class EntityTagRegistry final {
public:
  static constexpr const char *kDefaultRegistryPath = GameAssets::kTagRegistryJson;

  static EntityTagRegistry &Instance();

  /// 未読込なら JSON を読み込む（失敗時は組み込み既定へフォールバック）
  void EnsureLoaded();

  bool ReloadFromJsonPath(const std::string &jsonFilePath);

  const std::vector<std::string> &TagCatalog() const { return tagCatalog_; }

  /// roles の値（役割として選ぶタグ名）を tagCatalog の順で返す
  const std::vector<std::string> &RoleTagNames() const {
    return roleTagNames_;
  }

  std::string TagNameForRole(const std::string &roleKey) const;

  uint64_t HashForTagName(const std::string &tagName) const {
    return Hash64(tagName);
  }

  uint64_t HashForRole(const std::string &roleKey) const;

  bool EntityHasTagName(const Entity &entity, const std::string &tagName) const;

  bool EntityHasRole(const Entity &entity, const std::string &roleKey) const;

  bool EntityHasManagedBulletRole(const Entity &entity) const;

  void ApplyReflectParryTagConversion(Entity &entity) const;

  std::vector<uint64_t> PlayerDamageHurterTagHashes() const;
  std::vector<uint64_t> EnemyDamageHurterTagHashes() const;

  uint64_t DefaultParryReflectorTagHash() const;

private:
  EntityTagRegistry() = default;

  void ApplyDefaults();
  void RebuildDerivedCaches();

  std::vector<std::string> tagCatalog_{};
  std::vector<std::string> roleTagNames_{};
  std::vector<std::string> managedBulletTags_{};
  std::string reflectFromTag_{};
  std::string reflectToTag_{};
  std::vector<std::string> playerDamageHurterTags_{};
  std::vector<std::string> enemyDamageHurterTags_{};
  std::string parryReflectorRoleKey_{"playerWeapon"};
  std::unordered_map<std::string, std::string> rolesByKey_{};

  bool loaded_ = false;
};

} // namespace Atrum
