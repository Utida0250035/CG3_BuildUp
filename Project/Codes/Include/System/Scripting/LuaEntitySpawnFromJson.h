#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum::Script {

struct LuaEntitySpawnResult {
  bool success = false;
  std::string message{};
  int spawnedCount = 0;
};

/// JSON パスを実行環境向けに解決する（シーン／エンティティ定義の両方）
std::string ResolveLuaEntityJsonPath(const std::string &path);

/// 読み込んだ定義から Entity を組み立て、既存配列へ追記する（MainCamera 補完なし）
LuaEntitySpawnResult TryAppendEntitiesFromJsonPath(
    const std::string &jsonPath, std::vector<std::unique_ptr<Entity>> &entities,
    float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

} // namespace Atrum::Script