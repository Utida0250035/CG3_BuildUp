#pragma once
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#define DEFINE_JSON NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE

namespace Atrum::Json {

/// <summary>
/// JSON操作クラス 汎用 シリアライズ定義済み前提
/// </summary>
class JsonHandler {

private:
  using json = nlohmann::json;
  JsonHandler() = default;
  ~JsonHandler() = default;

  JsonHandler(const JsonHandler &) = delete;
  JsonHandler &operator=(const JsonHandler &) = delete;

  JsonHandler(JsonHandler &&) = delete;
  JsonHandler &operator=(JsonHandler &&) = delete;

public:
  /// <summary>
  /// jsonファイルからの読み込み
  /// </summary>
  /// <typeparam name="T"> 任意の構造体 </typeparam>
  /// <param name="fileName"></param>
  /// <returns></returns>
  template <typename T> static T LoadFromFile(const std::string &filePath) {

    std::ifstream file(filePath);
    if (!file.is_open()) {
      throw std::runtime_error("nlohmann: Json file not found");
    }

    json j;
    file >> j;

    file.close();

    return j.get<T>();
  }

  /// <summary>
  /// jsonファイルへの出力
  /// </summary>
  /// <typeparam name="T"></typeparam>
  /// <param name="fileName"></param>
  /// <param name="data"></param>
  template <typename T>
  static void SaveToFile(const std::string &filePath, const T &data) {

    std::filesystem::path path(filePath);

    std::filesystem::create_directories(path.parent_path());

    std::ofstream file(path);
    if (!file.is_open()) {
      throw std::runtime_error("nlohmann: Json file could not open");
    }

    json j = data;
    file << j.dump(4);
    file.flush();

    file.close();
  }
};

} // namespace Atrum::Json