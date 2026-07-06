#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <format>

void Log(const std::string& message);

class LogFile {

private:

	// ログ出力先ファイルのパス
	std::string filePath_ = "";

	std::ofstream logStream_;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	LogFile() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LogFile();

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ログ出力
	/// </summary>
	/// <param name="message"> 文字列 </param>
	void Log(const std::string& message);

	/// <summary>
	/// ログ出力
	/// </summary>
	/// <param name="message"> 文字列(wide) </param>
	void Log(const std::wstring& message);

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static LogFile* GetInstance() {

		static LogFile instance;

		return &instance;

	}

	/// <summary>
	/// 代入演算子の削除
	/// </summary>
	/// <param name="source"></param>
	/// <returns></returns>
	LogFile operator=(const LogFile& source) = delete;

	/// <summary>
	/// コピーコンストラクタの削除
	/// </summary>
	/// <param name="source"></param>
	LogFile(const LogFile& source) = delete;

};

namespace console {

	template<class... Args>
	void Println([[maybe_unused]]const std::format_string<Args...>& fmt, [[maybe_unused]]const Args&&... args) {

#ifdef _DEBUG

		std::cout << std::format(fmt, std::forward<Args>(args)...) << std::endl;

#endif

	}

}