#pragma once
#include <boost/pfr.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <type_traits>

namespace Atrum::Json {

    class PfrJsonHandler {

    private:
        using json = nlohmann::json;

        // 構造体をJSONに変換（ポインタ型を除外）
        template <typename T>
        static json ToJson(const T& obj) {
            json j;
            boost::pfr::for_each_field(obj, [&](const auto& field, std::size_t idx) {
                // ポインタ型は除外
                if constexpr (!std::is_pointer_v<std::decay_t<decltype(field)>>) {
                    // 構造体のメンバ名を自動取得（※）
                    // メンバ名を文字列として扱いたい場合、PFRはコンパイル時の名前に制約があるため注意
                    // ここでは簡単のため idx をキーにしていますが、実際には struct_name などを検討
                    j[std::to_string(idx)] = field;
                }
            });
            return j;
        }

        // JSONから構造体に変換（読み込み）
        template <typename T>
        static void FromJson(const json& j, T& obj) {
            boost::pfr::for_each_field(obj, [&](auto& field, std::size_t idx) {
                if constexpr (!std::is_pointer_v<std::decay_t<decltype(field)>>) {
                    if (j.contains(std::to_string(idx))) {
                        field = j[std::to_string(idx)].get<std::decay_t<decltype(field)>>();
                    }
                }
            });
        }

    public:

        /// <summary>
        /// jsonファイルからの読み込み
        /// </summary>
        /// <typeparam name="T"> 任意の構造体 </typeparam>
        /// <param name="fileName"></param>
        /// <returns></returns>
        template<typename T>
        static T LoadFromFile(const std::string& fileName) {

            std::ifstream file(fileName);
            if (!file.is_open()) {
                throw std::runtime_error("nlohmann: Json file not found");
            }

            json j;
            file >> j;

            T data{};

            FromJson(j, data);

            return data;
        }

        /// <summary>
        /// jsonファイルへの出力
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="fileName"></param>
        /// <param name="data"></param>
        template <typename T>
        static void SaveToFile(const std::string& fileName, const T& data) {

            std::ofstream file(fileName);
            json j = data;
            file << j.dump(4);

        }

    };

}