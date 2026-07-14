#pragma once

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

/// <summary>
/// JSON操作クラス 汎用 シリアライズ定義済み前提
/// </summary>
class JsonHandler {

private:
	using json = nlohmann::json;

public:

	/// <summary>
	/// 
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
		return j.get<T>();
	}

	template <typename T>
	static void SaveToFile(const std::string& fileName, const T& data) {

		std::ofstream file(fileName);
		json j = data;
		file << j.dump(4);

	}

};