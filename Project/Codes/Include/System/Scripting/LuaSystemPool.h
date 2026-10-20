#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Atrum::Script {

class LuaSystem;

// LuaSystemの複数管理用
class LuaSystemPool {
private:
  // LuaSystem配列
  std::vector<std::unique_ptr<LuaSystem>> luaSystems_{};
  // LuaSystemの名前
  std::unordered_map<uint64_t, size_t> map_{};

public:
  // 名前を付けて空のLuaSystemを追加
  void Add(const std::string &name);
  // LuaSystem配列の一括初期化
  bool InitSystems();
  // LuaSystem配列からの検索
  LuaSystem *Find(const std::string &name);

  LuaSystemPool() = default;
  ~LuaSystemPool() = default;
};
} // namespace Atrum::Script